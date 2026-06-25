#include "level_loader.h"
#include "../component/animation_component.h"
#include "../component/audio_component.h"
#include "../component/collider_component.h"
#include "../component/health_component.h"
#include "../component/parallax_component.h"
#include "../component/physics_component.h"
#include "../component/sprite_component.h"
#include "../component/tilelayer_component.h"
#include "../component/transform_component.h"
#include "../core/context.h"
#include "../object/game_object.h"
#include "../render/animation.h"
#include "../render/sprite.h"
#include "../scene/scene_base.h"

#include <glm/vec2.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <filesystem>
#include <fstream>

namespace engine::scene {

bool LevelLoader::loadLevel(std::string_view mapPath, SceneBase& scene)
{
    // 1. 加载关卡地图 JSON 文件
    auto path = std::filesystem::path{ mapPath };
    std::ifstream file{ path };
    if (!file.is_open()) {
        spdlog::error("无法打开关卡地图文件: {}", mapPath);
        return false;
    }

    // 2. 解析关卡地图 JSON 数据
    nlohmann::json mapJson;
    try {
        file >> mapJson;
    } catch (const nlohmann::json::parse_error& e) {
        spdlog::error("解析关卡地图 JSON 数据失败: {}", e.what());
        return false;
    }

    // 3. 获取基本地图信息（地图路径、地图尺寸、瓦片尺寸）
    m_mapPath = mapPath;
    m_mapSize = glm::ivec2{ mapJson.value("width", 0), mapJson.value("height", 0) };
    m_tileSize = glm::ivec2{ mapJson.value("tilewidth", 0), mapJson.value("tileheight", 0) };

    // 4. 加载瓦片集数据
    if (mapJson.contains("tilesets") && mapJson["tilesets"].is_array()) {
        for (const auto& tileset : mapJson["tilesets"]) {
            auto tilesetPath = resolvePath(tileset["source"].get<std::string>(), m_mapPath);
            auto firstGid = tileset["firstgid"].get<int>();
            loadTileset(tilesetPath, firstGid);
        }
    }

    // 5. 加载图层数据
    if (!mapJson.contains("layers") || !mapJson["layers"].is_array()) {
        // 地图文件中必须有 layers 数组
        spdlog::error("地图文件 '{}' 中缺少或无效的 'layers' 数组。", mapPath);
        return false;
    }

    for (const auto& layer : mapJson["layers"]) {
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
    const std::string imagePath{ layerJson.value("image", "") };
    if (imagePath.empty()) {
        spdlog::error("图层 '{}' 缺少 'image' 属性。", layerJson.value("name", "Unnamed"));
        return;
    }
    auto textureId = resolvePath(imagePath, m_mapPath);

    // 获取图层偏移量（json中没有则代表未设置，给默认值即可）
    const glm::vec2 offset{ glm::vec2{ layerJson.value("offsetx", 0.0F),
                                       layerJson.value("offsety", 0.0F) } };

    // 获取视差因子及重复标志
    const glm::vec2 scrollFactor{ glm::vec2{ layerJson.value("parallaxx", 1.0F),
                                             layerJson.value("parallaxy", 1.0F) } };
    const glm::bvec2 repeat{ glm::bvec2{ layerJson.value("repeatx", false),
                                         layerJson.value("repeaty", false) } };

    // 获取图层名称
    std::string layerName{ layerJson.value("name", "Unnamed") };

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
                   [this](const auto& gid) { return getTileInfoByGid(gid); });

    // 获取图层名称
    std::string layerName{ layerJson.value("name", "Unnamed") };
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

        if (gid == 0) { // 如果 gid 为 0，代表是自定义形状，如碰撞盒，我们以后再处理
            // 非矩形对象会有额外标识（目前不考虑）
            if (object.value("point", false)) {          // 如果是点对象
                continue;                                // TODO: 点对象的处理方式
            } else if (object.value("ellipse", false)) { // 如果是椭圆对象
                continue;                                // TODO: 椭圆对象的处理方式
            } else if (object.value("polygon", false)) { // 如果是多边形对象
                continue;                                // TODO: 多边形对象的处理方式
            }
            // 没有这些标识则默认是矩形对象
            else {
                // --- 创建游戏对象并添加TransfromComponent ---
                std::string objectName = object.value("name", "Unnamed");
                auto gameObject = std::make_unique<engine::object::GameObject>(objectName);
                // 获取Transform相关信息 （自定义形状的坐标针对左上角）
                auto position = glm::vec2{ object.value("x", 0.0F), object.value("y", 0.0F) };
                auto dstRectSize = glm::vec2{ object.value("width", 0.0F),
                                              object.value("height", 0.0F) };
                auto rotation = object.value("rotation", 0.0F);
                // 添加TransformComponent，缩放为设定为1.0F
                gameObject->addComponent<engine::component::TransformComponent>(position,
                                                                                glm::vec2{ 1.0F },
                                                                                rotation);

                // --- 添加碰撞组件和物理组件 ---
                // 碰撞盒大小与dstRectSize相同
                auto collider = std::make_unique<engine::physics::AabbCollider>(dstRectSize);
                auto* colliderComponent = gameObject
                                              ->addComponent<engine::component::ColliderComponent>(
                                                  std::move(collider));
                // 自定义形状通常是trigger类型，除非显示指定 （因此默认为真）
                colliderComponent->setTrigger(object.value("trigger", true));
                // 添加物理组件，不受重力影响
                gameObject->addComponent<engine::component::PhysicsComponent>(&scene.context()
                                                                                   .physicsEngine(),
                                                                              false);

                // -- 根据瓦片自定义属性设置 GameObject --
                // 获取标签信息并设置
                if (auto tag = getTileProperty<std::string>(object, "tag"); tag) { // 如果有标签
                    gameObject->setTag(tag.value());
                }

                // 添加到场景
                scene.addGameObject(std::move(gameObject));
                spdlog::info("加载对象: '{}' 完成 (类型: 自定义形状)", objectName);
            }
        } else { // 如果 gid 存在，则代表这是一个带图像的对象
            // -- 添加 Transform、Sprite 组件 --
            // 获取瓦片信息
            auto tileInfo = getTileInfoByGid(gid);
            if (tileInfo.sprite.textureId().empty()) {
                spdlog::error("gid为 {} 的瓦片没有图像纹理。", gid);
                continue;
            }

            // 获取构建 Transform 组件所需的信息
            // 1. 获取对象位置
            auto position = glm::vec2{ object.value("x", 0.0F), object.value("y", 0.0F) };
            auto dstRectSize = glm::vec2{ object.value("width", 0.0F),
                                          object.value("height", 0.0F) };

            /**  !! 关键的坐标转换 !!
             * 从 Tiled 中获取的坐标是左下角，而 SDL 游戏引擎的坐标是左上角，所以需要转换
             */
            position = glm::vec2{ position.x, position.y - dstRectSize.y };

            // 2. 获取对象旋转角度
            auto rotation = object.value("rotation", 0.0F);

            // 3. 计算缩放比例
            auto srcRect = tileInfo.sprite.sourceRect();
            if (!srcRect) {
                spdlog::error("gid为 {} 的瓦片没有源矩形。", gid);
                continue;
            }
            auto srcRectSize = glm::vec2{ srcRect->w, srcRect->h };
            auto scale = dstRectSize / srcRectSize;

            // 获取对象名称
            std::string objectName{ object.value("name", "Unnamed") };

            // 创建 GameObject 并添加组件
            auto gameObject = std::make_unique<engine::object::GameObject>(objectName);
            gameObject->addComponent<engine::component::TransformComponent>(position,
                                                                            scale,
                                                                            rotation);
            gameObject->addComponent<engine::component::SpriteComponent>(std::move(tileInfo.sprite),
                                                                         scene.context()
                                                                             .resourceManager());

            // -- 添加碰撞器、物理组件 --
            /** 获取瓦片json信息
             * 1. 必然存在，因为 getTileInfoByGid(gid) 函数已经顺利执行
             * 2. 这里再获取 tile JSON，实际上检索了两次，未来可以优化
             */
            auto tileJson = getTileJsonByGid(gid);

            // 如果是 Solid 类型，则直接添加碰撞器组件，且图片源矩形区域就是碰撞盒大小
            if (tileInfo.type == engine::component::TileType::Solid) {
                auto collider = std::make_unique<engine::physics::AabbCollider>(srcRectSize);
                gameObject->addComponent<engine::component::ColliderComponent>(std::move(collider));
                // 添加物理组件（默认不受重力影响）
                gameObject->addComponent<engine::component::PhysicsComponent>(&scene.context()
                                                                                   .physicsEngine(),
                                                                              false);
                // 设置标签方便物理引擎检索
                gameObject->setTag("solid");
            }
            // 如果非 Solid 类型，检查自定义碰撞盒是否存在
            else if (auto rect = getColliderRect(tileJson); rect) {
                // 如果有，添加碰撞器组件
                auto collider = std::make_unique<engine::physics::AabbCollider>(rect->size);
                auto* colliderComponent = gameObject
                                              ->addComponent<engine::component::ColliderComponent>(
                                                  std::move(collider));
                // 自定义碰撞盒的坐标是相对于图片坐标的偏移量，也就是针对 TransformComponent 的偏移量
                colliderComponent->setOffset(rect->position);
                // 添加物理组件（默认不受重力影响）
                gameObject->addComponent<engine::component::PhysicsComponent>(&scene.context()
                                                                                   .physicsEngine(),
                                                                              false);
            }

            // -- 根据瓦片自定义属性设置 GameObject --
            // 获取标签信息并设置
            auto tag = getTileProperty<std::string>(tileJson, "tag");
            if (tag) {
                gameObject->setTag(tag.value());
            } else if (tileInfo.type == engine::component::TileType::Hazard) {
                // 如果是危险瓦片，且没有手动设置标签，则自动设置标签为 "hazard"
                gameObject->setTag("hazard");
            }

            // 获取重力信息并设置
            auto gravity = getTileProperty<bool>(tileJson, "gravity");
            if (gravity) {
                auto* physicsComponent = gameObject
                                             ->getComponent<engine::component::PhysicsComponent>();
                if (physicsComponent != nullptr) {
                    physicsComponent->setGravityEnabled(gravity.value());
                } else {
                    spdlog::warn("对象 '{}' 在设置重力信息时没有物理组件，请检查地图设置。",
                                 objectName);
                    gameObject->addComponent<engine::component::PhysicsComponent>(
                        &scene.context().physicsEngine(), gravity.value());
                }
            }

            // 获取动画信息并设置
            auto animation = getTileProperty<std::string>(tileJson, "animation");
            if (animation) {
                // 解析 string 到 JSON 对象
                nlohmann::json animationJson;
                try {
                    animationJson = nlohmann::json::parse(animation.value());
                } catch (const nlohmann::json::parse_error& e) {
                    spdlog::error("解析动画 JSON 字符串失败：{}", e.what());
                    continue; // 跳过当前对象，继续处理下一个对象
                }
                // 添加组件到 GameObject
                auto* animationComponent
                    = gameObject->addComponent<engine::component::AnimationComponent>();
                // 添加动画到组件
                addAnimation(animationJson, animationComponent, srcRectSize);
            }

            // 获取音效信息并设置
            auto sound = getTileProperty<std::string>(tileJson, "sound");
            if (sound) {
                // 解析 string 到 JSON 对象
                nlohmann::json soundJson;
                try {
                    soundJson = nlohmann::json::parse(sound.value());
                } catch (const nlohmann::json::parse_error& e) {
                    spdlog::error("解析音效 JSON 字符串失败：{}", e.what());
                    continue; // 跳过当前对象，继续处理下一个对象
                }
                // 添加组件到 GameObject
                auto* audioComponent = gameObject->addComponent<engine::component::AudioComponent>(
                    &scene.context().audioPlayer(), &scene.context().camera());
                // 添加音效到组件
                addSound(soundJson, audioComponent);
            }

            // 获取生命值信息并设置
            auto health = getTileProperty<int>(tileJson, "health");
            if (health) {
                gameObject->addComponent<engine::component::HealthComponent>(health.value());
            }

            // -- 添加 GameObject 到场景中 --
            scene.addGameObject(std::move(gameObject));
            spdlog::info("加载对象: '{}' 完成", objectName);
        }
    }
}

void LevelLoader::addAnimation(const nlohmann::json& animationJson,
                               engine::component::AnimationComponent* animationComponent,
                               const glm::vec2& spriteSize)
{
    // 检查 animationJson 必须是一个对象，并且 animationComponent 不能为 nullptr
    if (!animationJson.is_object() || animationComponent == nullptr) {
        spdlog::error("无效的动画 JSON 或 AnimationComponent 指针。");
        return;
    }

    // 遍历动画 JSON 对象中的每个键值对（动画名称 : 动画信息）
    for (const auto& anime : animationJson.items()) {
        std::string_view animeName{ anime.key() };
        const auto& animeInfo{ anime.value() };
        if (!animeInfo.is_object()) {
            spdlog::warn("动画 '{}' 的信息无效或为空。", animeName);
            continue;
        }

        // 1. 获取可能存在的动画帧信息
        auto duration_ms = animeInfo.value("duration", 100);       // 默认持续时间为100毫秒
        auto duration = static_cast<float>(duration_ms) / 1000.0F; // 转换为秒
        auto row = animeInfo.value("row", 0);                      // 默认行数为0
        // 2. 帧信息（数组）是必须存在的
        if (!animeInfo.contains("frames") || !animeInfo["frames"].is_array()) {
            spdlog::warn("动画 '{}' 缺少 'frames' 数组。", animeName);
            continue;
        }

        // 3. 创建一个Animation对象 (默认为循环播放)
        auto animation = std::make_unique<engine::render::Animation>(animeName);

        // 4. 遍历数组并进行添加帧信息到animation对象
        for (const auto& frame : animeInfo["frames"]) {
            if (!frame.is_number_integer()) {
                spdlog::warn("动画 {} 中 frames 数组格式错误！", animeName);
                continue;
            }

            auto column = frame.get<int>();
            // 计算源矩形
            const SDL_FRect srcRect{
                column * spriteSize.x, row * spriteSize.y, spriteSize.x, spriteSize.y
            };
            // 添加动画帧到 Animation
            animation->addFrame(srcRect, duration);
        }

        // 5. 将 Animation 对象添加到 AnimationComponent 中
        animationComponent->addAnimation(std::move(animation));
    }
}

void LevelLoader::addSound(const nlohmann::json& soundJson,
                           engine::component::AudioComponent* audioComponent)
{
    // 检查 soundJson 必须是一个对象，并且 audioComponent 不能为 nullptr
    if (!soundJson.is_object() || audioComponent == nullptr) {
        spdlog::error("无效的音效 JSON 或 AudioComponent 指针。");
        return;
    }

    // 遍历音效 JSON 对象中的每个键值对（音效 ID : 音效路径）
    for (const auto& sound : soundJson.items()) {
        std::string_view soundId{ sound.key() };
        const std::string soundPath{ sound.value().get<std::string>() };
        if (soundId.empty() || soundPath.empty()) {
            spdlog::warn("音效 '{}' 缺少必要信息。", soundId);
            continue;
        }
        // 添加音效到 AudioComponent
        audioComponent->addSound(soundId, soundPath);
    }
}

std::optional<engine::utils::Rect> LevelLoader::getColliderRect(const nlohmann::json& tileJson)
{
    if (!tileJson.contains("objectgroup")) {
        return std::nullopt;
    }

    const auto& objectGroup = tileJson["objectgroup"];
    if (!objectGroup.contains("objects")) {
        return std::nullopt;
    }

    // 一个图片只支持一个碰撞器。如果有多个，就返回第一个不为空的碰撞器
    for (const auto& object : objectGroup["objects"]) {
        auto rect = engine::utils::Rect{
            glm::vec2{ object.value("x", 0.0F), object.value("y", 0.0F) },
            glm::vec2{ object.value("width", 0.0F), object.value("height", 0.0F) }
        };
        if (rect.size.x > 0.0F && rect.size.y > 0.0F) {
            return rect;
        }
    }

    // 如果没有找到不为空的碰撞器，就返回空
    return std::nullopt;
}

engine::component::TileType LevelLoader::getTileType(const nlohmann::json& tileJson)
{
    if (tileJson.contains("properties")) {
        for (const auto& property : tileJson["properties"]) {
            if (property.value("name", "") == "solid") {
                return property.value("value", false) ? engine::component::TileType::Solid
                                                      : engine::component::TileType::Normal;
            } else if (property.value("name", "") == "unisolid") {
                return property.value("value", false) ? engine::component::TileType::Unisolid
                                                      : engine::component::TileType::Normal;
            } else if (property.value("name", "") == "slope") {
                auto slopeType = property.value("value", "");
                if (slopeType == "0_1") {
                    return engine::component::TileType::Slope_0_1;
                } else if (slopeType == "1_0") {
                    return engine::component::TileType::Slope_1_0;
                } else if (slopeType == "0_2") {
                    return engine::component::TileType::Slope_0_2;
                } else if (slopeType == "2_1") {
                    return engine::component::TileType::Slope_2_1;
                } else if (slopeType == "1_2") {
                    return engine::component::TileType::Slope_1_2;
                } else if (slopeType == "2_0") {
                    return engine::component::TileType::Slope_2_0;
                } else {
                    spdlog::error("未知的斜坡类型: {}", slopeType);
                    return engine::component::TileType::Normal;
                }
            } else if (property.value("name", "") == "hazard") {
                return property.value("value", false) ? engine::component::TileType::Hazard
                                                      : engine::component::TileType::Normal;
            } else if (property.value("name", "") == "ladder") {
                return property.value("value", false) ? engine::component::TileType::Ladder
                                                      : engine::component::TileType::Normal;
            }
            // TODO: 可以在这里添加更多的自定义属性处理逻辑
        }
    }

    return engine::component::TileType::Normal;
}

engine::component::TileType LevelLoader::getTileTypeById(const nlohmann::json& tilesetJson,
                                                         int localId)
{
    if (tilesetJson.contains("tiles")) {
        const auto& tiles = tilesetJson["tiles"];
        auto it = std::find_if(tiles.begin(), tiles.end(), [localId](const auto& tile) {
            return tile.value("id", -1) == localId;
        });

        if (it != tiles.end()) {
            return getTileType(*it);
        }
    }

    return engine::component::TileType::Normal;
}

engine::component::TileInfo LevelLoader::getTileInfoByGid(int gid)
{
    if (gid == 0) {
        return engine::component::TileInfo{};
    }

    // upper_bound：查找 tilesets 中键大于 gid 的第一个元素，返回迭代器
    auto it = m_tilesets.upper_bound(gid);
    if (it == m_tilesets.begin()) {
        spdlog::error("gid为 {} 的瓦片未找到图块集。", gid);
        return engine::component::TileInfo{};
    }
    --it; // 前移一个位置，这样就得到不大于gid的最近一个元素（我们需要的）

    const auto& tilesetJson = it->second;
    const auto& tilesetFirstGid = it->first;

    const std::string filePath{ tilesetJson.value("filePath", "") }; // 获取图块集文件路径
    if (filePath.empty()) {
        spdlog::error("Tileset 文件 '{}' 缺少 'filePath' 属性。", tilesetFirstGid);
        return engine::component::TileInfo{};
    }

    auto localId = gid - tilesetFirstGid; // 计算瓦片在图块集中的局部 ID

    // 图块集分为两种情况，需要分别考虑
    if (tilesetJson.contains("image")) { // 这是单一图片的情况
        // 获取图片路径
        auto textureId = resolvePath(tilesetJson["image"].get<std::string>(), filePath);
        // 计算在图片网格中的瓦片坐标
        auto coordinateX = localId % tilesetJson["columns"].get<int>();
        auto coordinateY = localId / tilesetJson["columns"].get<int>();
        // 根据瓦片坐标确定源矩形
        SDL_FRect srcRect{ static_cast<float>(coordinateX * m_tileSize.x),
                           static_cast<float>(coordinateY * m_tileSize.y),
                           static_cast<float>(m_tileSize.x),
                           static_cast<float>(m_tileSize.y) };
        // 创建瓦片精灵
        const engine::render::Sprite sprite{ textureId, srcRect };
        // 获取瓦片类型
        auto tileType = getTileTypeById(tilesetJson, localId);
        // 返回瓦片信息
        return engine::component::TileInfo{ sprite, tileType };
    } else { // 这是多图片的情况
        // 没有 tiles 字段的话不符合数据格式要求，直接返回空的瓦片信息
        if (!tilesetJson.contains("tiles")) {
            spdlog::error("Tileset 文件 '{}' 缺少 'tiles' 属性。", tilesetFirstGid);
            return engine::component::TileInfo{};
        }

        // 遍历 tileset JSON 中的 tiles 数组，根据局部 ID 查找对应的瓦片
        for (const auto& tile : tilesetJson["tiles"]) {
            auto tileId = tile.value("id", 0);
            if (tileId == localId) { // 找到对应的瓦片，进行后续操作
                // 没有 image 字段的话不符合数据格式要求，直接返回空的瓦片信息
                if (!tile.contains("image")) {
                    spdlog::error("Tileset 文件 '{}' 中瓦片 {} 缺少 'image' 属性。",
                                  tilesetFirstGid,
                                  tileId);
                    return engine::component::TileInfo{};
                }

                // --- 接下来根据必要信息创建并返回 TileInfo ---
                // 获取图片路径
                auto textureId = resolvePath(tile["image"].get<std::string>(), filePath);
                // 先确认图片尺寸
                auto imageWidth = tile.value("imagewidth", 0);
                auto imageHeight = tile.value("imageheight", 0);
                /** 从 tile JSON 中获取源矩形信息
                 * Tiled 中源矩形信息只有设置了才会有值，没有就是默认值
                 * 如果未设置，则使用图片尺寸
                 */
                SDL_FRect srcRect{ static_cast<float>(tile.value("x", 0)),
                                   static_cast<float>(tile.value("y", 0)),
                                   static_cast<float>(tile.value("width", imageWidth)),
                                   static_cast<float>(tile.value("height", imageHeight)) };
                // 创建瓦片精灵
                const engine::render::Sprite sprite{ textureId, srcRect };
                // 获取瓦片类型
                auto tileType = getTileType(tile);
                // 返回瓦片信息
                return engine::component::TileInfo{ sprite, tileType };
            }
        }
    }

    // 如果能走到这里，说明查找失败，返回空的瓦片信息
    spdlog::error("Tileset 文件 '{}' 中未找到gid为 {} 的瓦片。", tilesetFirstGid, gid);
    return engine::component::TileInfo{};
}

std::optional<nlohmann::json> LevelLoader::getTileJsonByGid(int gid) const
{
    // 1. 查找 tilesets 中键大于 gid 的第一个元素，返回迭代器
    auto it = m_tilesets.upper_bound(gid);
    if (it == m_tilesets.begin()) {
        spdlog::error("gid为 {} 的瓦片未找到图块集。", gid);
        return std::nullopt;
    }
    --it; // 前移一个位置，这样就得到不大于gid的最近一个元素（我们需要的）

    // 2. 获取 tileset JSON 对象
    const auto& tilesetJson = it->second;
    const auto& tilesetFirstGid = it->first;
    // 没有 tiles 字段的话不符合数据格式要求，直接返回空
    if (!tilesetJson.contains("tiles")) {
        spdlog::error("Tileset 文件 '{}' 缺少 'tiles' 属性。", tilesetFirstGid);
        return std::nullopt;
    }

    // 3. 遍历 tileset JSON 中的 tiles 数组，根据局部 ID 查找对应的瓦片, 并返回瓦片 JSON 对象
    auto localId = gid - tilesetFirstGid; // 计算瓦片在图块集中的局部ID
    for (const auto& tile : tilesetJson["tiles"]) {
        auto tileId = tile.value("id", 0);
        if (tileId == localId) {
            return std::make_optional(tile);
        }
    }

    return std::nullopt;
}

void LevelLoader::loadTileset(std::string_view tilesetPath, int firstGid)
{
    auto path = std::filesystem::path{ tilesetPath };
    std::ifstream file{ path };
    if (!file.is_open()) {
        spdlog::error("无法打开瓦片集文件: {}", tilesetPath);
        return;
    }

    nlohmann::json tilesetJson;
    file >> tilesetJson;

    // 注入瓦片集文件路径，方便后续加载瓦片时解析相对路径
    tilesetJson["filePath"] = tilesetPath;

    m_tilesets[firstGid] = std::move(tilesetJson);
}

std::string LevelLoader::resolvePath(std::string_view relativePath, std::string_view filePath)
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
        return std::string(relativePath);
    }
}

} // namespace engine::scene
