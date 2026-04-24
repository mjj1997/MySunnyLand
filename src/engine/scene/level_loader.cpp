#include "level_loader.h"
#include "../component/parallax_component.h"
#include "../component/sprite_component.h"
#include "../component/tilelayer_component.h"
#include "../component/transform_component.h"
#include "../core/context.h"
#include "../object/game_object.h"
#include "../render/sprite.h"
#include "../scene/scene_base.h"

#include <glm/vec2.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <filesystem>
#include <fstream>

namespace engine::scene {

bool LevelLoader::loadLevel(const std::string& mapPath, SceneBase& scene)
{
    // 1. 加载 JSON 文件
    std::ifstream file{ mapPath };
    if (!file.is_open()) {
        spdlog::error("无法打开关卡文件: {}", mapPath);
        return false;
    }

    // 2. 解析 JSON 数据
    nlohmann::json jsonData;
    try {
        file >> jsonData;
    } catch (const nlohmann::json::parse_error& e) {
        spdlog::error("解析 JSON 数据失败: {}", e.what());
        return false;
    }

    // 3. 获取基本地图信息（地图路径、地图尺寸、瓦片尺寸）
    m_mapPath = mapPath;
    m_mapSize = glm::ivec2{ jsonData.value("width", 0), jsonData.value("height", 0) };
    m_tileSize = glm::ivec2{ jsonData.value("tilewidth", 0), jsonData.value("tileheight", 0) };

    // 4. 加载瓦片集数据
    if (jsonData.contains("tilesets") && jsonData["tilesets"].is_array()) {
        for (const auto& tileset : jsonData["tilesets"]) {
            auto tilesetPath = resolvePath(tileset["source"].get<std::string>(), m_mapPath);
            auto firstGid = tileset["firstgid"].get<int>();
            loadTileset(tilesetPath, firstGid);
        }
    }

    // 5. 加载图层数据
    if (!jsonData.contains("layers") || !jsonData["layers"].is_array()) {
        // 地图文件中必须有 layers 数组
        spdlog::error("地图文件 '{}' 中缺少或无效的 'layers' 数组。", mapPath);
        return false;
    }

    for (const auto& layer : jsonData["layers"]) {
        // 获取各图层对象中的类型（type）字段
        std::string type{ layer.value("type", "none") };
        if (!layer.value("visible", true)) {
            spdlog::info("图层 '{}' 不可见，跳过加载。", layer.value("name", "Unnamed"));
            continue;
        }

        // 根据图层类型决定加载方法
        if (type == "imagelayer") {
            loadImageLayer(layer, scene);
        } else if (type == "tilelayer") {
            loadTileLayer(layer, scene);
        } else if (type == "objectgroup") {
            loadObjectLayer(layer, scene);
        } else {
            spdlog::warn("不支持的图层类型: {}", type);
        }
    }

    spdlog::info("关卡加载完成: {}", mapPath);
    return true;
}

void LevelLoader::loadImageLayer(const nlohmann::json& layerJson, SceneBase& scene)
{
    // 获取纹理相对路径 （会自动处理'\/'符号）
    const std::string& imagePath{ layerJson.value("image", "") };
    if (imagePath.empty()) {
        spdlog::error("图层 '{}' 缺少 'image' 属性。", layerJson.value("name", "Unnamed"));
        return;
    }
    auto textureId = resolvePath(imagePath, m_mapPath);

    // 获取图层偏移量（json中没有则代表未设置，给默认值即可）
    const glm::vec2 offset{ glm::vec2{ layerJson.value("offsetx", 0.0f),
                                       layerJson.value("offsety", 0.0f) } };

    // 获取视差因子及重复标志
    const glm::vec2 scrollFactor{ glm::vec2{ layerJson.value("parallaxx", 1.0f),
                                             layerJson.value("parallaxy", 1.0f) } };
    const glm::bvec2 repeat{ glm::bvec2{ layerJson.value("repeatx", false),
                                         layerJson.value("repeaty", false) } };

    // 获取图层名称
    const std::string& layerName{ layerJson.value("name", "Unnamed") };

    /*  可用类似方法获取其它各种属性，这里我们暂时用不上 */

    // 创建游戏对象
    auto gameObject = std::make_unique<engine::object::GameObject>(layerName);
    // 依次添加Transform，Parallax组件
    gameObject->addComponent<engine::component::TransformComponent>(offset);
    gameObject->addComponent<engine::component::ParallaxComponent>(textureId, scrollFactor, repeat);
    // 添加到场景中
    scene.addGameObject(std::move(gameObject));
    spdlog::info("加载图层: '{}' 完成", layerName);
}

void LevelLoader::loadTileLayer(const nlohmann::json& layerJson, SceneBase& scene)
{
    if (!layerJson.contains("data") || !layerJson["data"].is_array()) {
        spdlog::error("图层 '{}' 缺少 'data' 属性。", layerJson.value("name", "Unnamed"));
        return;
    }

    // 准备 TileInfo Vector (瓦片数量 = 地图宽度 * 地图高度)
    std::vector<engine::component::TileInfo> tiles;
    tiles.reserve(m_mapSize.x * m_mapSize.y);

    // 根据gid获取必要信息，并依次填充 TileInfo Vector
    std::transform(layerJson["data"].begin(),
                   layerJson["data"].end(),
                   std::back_inserter(tiles),
                   [this](const auto& gid) { return tileInfoByGid(gid); });

    // 获取图层名称
    const std::string& layerName{ layerJson.value("name", "Unnamed") };
    // 创建游戏对象
    auto gameObject = std::make_unique<engine::object::GameObject>(layerName);
    // 添加Tilelayer组件
    gameObject->addComponent<engine::component::TileLayerComponent>(m_tileSize,
                                                                    m_mapSize,
                                                                    std::move(tiles));
    // 添加到场景中
    scene.addGameObject(std::move(gameObject));
    spdlog::info("加载图层: '{}' 完成", layerName);
}

void LevelLoader::loadObjectLayer(const nlohmann::json& layerJson, SceneBase& scene)
{
    if (!layerJson.contains("objects") || !layerJson["objects"].is_array()) {
        spdlog::error("对象图层 '{}' 缺少 'objects' 属性。", layerJson.value("name", "Unnamed"));
        return;
    }

    for (const auto& object : layerJson["objects"]) {
        auto gid = object.value("gid", 0);

        if (gid == 0) {
            // 如果gid为0，代表是自定义形状，如碰撞盒，我们以后再处理
            // TODO: Handle shapes
        } else {
            // 如果gid存在，则代表这是一个带图像的对象
            auto tileInfo = tileInfoByGid(gid);
            if (tileInfo.sprite.textureId().empty()) {
                spdlog::error("gid为 {} 的瓦片没有图像纹理。", gid);
                continue;
            }

            // 1. 获取Transform信息
            auto position = glm::vec2{ object.value("x", 0.0f), object.value("y", 0.0f) };
            auto dstRectSize = glm::vec2{ object.value("width", 0.0f),
                                          object.value("height", 0.0f) };

            // !! 关键的坐标转换 !!
            // 从 Tiled 中获取的坐标是左下角，而 SDL 游戏引擎的坐标是左上角，所以需要转换
            position = glm::vec2{ position.x, position.y - dstRectSize.y };

            auto rotation = object.value("rotation", 0.0f);

            // 2. 计算缩放
            auto srcRect = tileInfo.sprite.sourceRect();
            if (!srcRect) {
                spdlog::error("gid为 {} 的瓦片没有源矩形。", gid);
                continue;
            }
            auto srcRectSize = glm::vec2{ srcRect->w, srcRect->h };
            auto scale = dstRectSize / srcRectSize;

            // 3. 获取对象名称
            const std::string& objectName{ object.value("name", "Unnamed") };

            // 4. 创建GameObject并添加组件
            auto gameObject = std::make_unique<engine::object::GameObject>(objectName);
            gameObject->addComponent<engine::component::TransformComponent>(position,
                                                                            scale,
                                                                            rotation);
            gameObject->addComponent<engine::component::SpriteComponent>(std::move(tileInfo.sprite),
                                                                         scene.context()
                                                                             .resourceManager());

            // 5. 添加到场景中
            scene.addGameObject(std::move(gameObject));
            spdlog::info("加载对象: '{}' 完成", objectName);
        }
    }
}

std::string LevelLoader::resolvePath(const std::string& relativePath, const std::string& filePath)
{
    try {
        // 获取地图/瓦片集文件的父目录（相对于可执行文件） "assets/maps/level1.tmj" -> "assets/maps"
        auto fileDir = std::filesystem::path(filePath).parent_path();
        // 合并路径（相对于可执行文件）并返回。
        /* std::filesystem::canonical：解析路径中的当前目录（.）和上级目录（..）导航符，得到一个干净的路径 */
        auto finalPath = std::filesystem::canonical(fileDir / relativePath);
        return finalPath.string();
    } catch (const std::exception& e) {
        spdlog::error("解析路径失败: {}", e.what());
        return relativePath;
    }
}

engine::component::TileInfo LevelLoader::tileInfoByGid(int gid)
{
    if (gid == 0) {
        return engine::component::TileInfo{};
    }

    // upper_bound：查找 tileSets 中键大于 gid 的第一个元素，返回迭代器
    auto it = m_tileSets.upper_bound(gid);
    if (it == m_tileSets.begin()) {
        spdlog::error("gid为 {} 的瓦片未找到图块集。", gid);
        return engine::component::TileInfo{};
    }
    --it; // 前移一个位置，这样就得到不大于gid的最近一个元素（我们需要的）

    const auto& tileSetData = it->second;
    const auto& tileSetFirstGid = it->first;
    const std::string filePath = tileSetData.value("filePath", ""); // 获取图块集文件路径
    if (filePath.empty()) {
        spdlog::error("Tileset 文件 '{}' 缺少 'filePath' 属性。", tileSetFirstGid);
        return engine::component::TileInfo{};
    }

    auto localId = gid - tileSetFirstGid; // 计算瓦片在图块集中的局部ID
    // 图块集分为两种情况，需要分别考虑
    if (tileSetData.contains("image")) { // 这是单一图片的情况
        // 获取图片路径
        auto textureId = resolvePath(tileSetData["image"].get<std::string>(), filePath);
        // 计算瓦片在图片网格中的坐标
        auto coordinateX = localId % tileSetData["columns"].get<int>();
        auto coordinateY = localId / tileSetData["columns"].get<int>();
        // 根据坐标确定源矩形
        SDL_FRect srcRect{ static_cast<float>(coordinateX * m_tileSize.x),
                           static_cast<float>(coordinateY * m_tileSize.y),
                           static_cast<float>(m_tileSize.x),
                           static_cast<float>(m_tileSize.y) };
        engine::render::Sprite sprite{ textureId, srcRect };
        // 目前只完成渲染，以后再考虑瓦片类型
        return engine::component::TileInfo{ sprite, engine::component::TileType::Normal };
    } else { // 这是多图片的情况
        if (!tileSetData.contains("tiles")) {
            // 没有tiles字段的话不符合数据格式要求，直接返回空的瓦片信息
            spdlog::error("Tileset 文件 '{}' 缺少 'tiles' 属性。", tileSetFirstGid);
            return engine::component::TileInfo{};
        }

        // 遍历tiles数组，根据id查找对应的瓦片
        for (const auto& tile : tileSetData["tiles"]) {
            auto tileId = tile.value("id", 0);
            if (tileId == localId) { // 找到对应的瓦片，进行后续操作
                if (!tile.contains("image")) {
                    // 没有image字段的话不符合数据格式要求，直接返回空的瓦片信息
                    spdlog::error("Tileset 文件 '{}' 中瓦片 {} 缺少 'image' 属性。",
                                  tileSetFirstGid,
                                  tileId);
                    return engine::component::TileInfo{};
                }

                // --- 接下来根据必要信息创建并返回 TileInfo ---
                // 获取图片路径
                auto textureId = resolvePath(tile["image"].get<std::string>(), filePath);
                // 先确认图片尺寸
                auto imageWidth = tile.value("imagewidth", 0);
                auto imageHeight = tile.value("imageheight", 0);
                // 从json中获取源矩形信息
                // tiled中源矩形信息只有设置了才会有值，没有就是默认值
                SDL_FRect srcRect{ static_cast<float>(tile.value("x", 0)),
                                   static_cast<float>(tile.value("y", 0)),
                                   // 如果未设置，则使用图片尺寸
                                   static_cast<float>(tile.value("width", imageWidth)),
                                   static_cast<float>(tile.value("height", imageHeight)) };
                engine::render::Sprite sprite{ textureId, srcRect };
                // 目前只完成渲染，以后再考虑瓦片类型
                return engine::component::TileInfo{ sprite, engine::component::TileType::Normal };
            }
        }
    }

    // 如果能走到这里，说明查找失败，返回空的瓦片信息
    spdlog::error("Tileset 文件 '{}' 中未找到gid为 {} 的瓦片。", tileSetFirstGid, gid);
    return engine::component::TileInfo{};
}

void LevelLoader::loadTileset(const std::string& tileSetPath, int firstGid)
{
    std::ifstream file{ tileSetPath };
    if (!file.is_open()) {
        spdlog::error("无法打开瓦片集文件: {}", tileSetPath);
        return;
    }

    nlohmann::json tileSetData;
    file >> tileSetData;

    // 注入瓦片集文件路径，方便后续加载瓦片时解析相对路径
    tileSetData["filePath"] = tileSetPath;

    m_tileSets[firstGid] = std::move(tileSetData);
}

} // namespace engine::scene
