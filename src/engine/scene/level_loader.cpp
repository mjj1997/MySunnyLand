#include "level_loader.h"
#include "../component/parallax_component.h"
#include "../component/transform_component.h"
#include "../core/context.h"
#include "../object/game_object.h"
#include "../render/sprite.h"
#include "../scene/scene_base.h"

#include <glm/vec2.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>

namespace engine::scene {

bool LevelLoader::loadLevel(const std::string& levelPath, SceneBase& scene)
{
    m_mapPath = levelPath;

    // 1. 加载 JSON 文件
    std::ifstream file{ levelPath };
    if (!file.is_open()) {
        spdlog::error("无法打开关卡文件: {}", levelPath);
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

    // 3. 加载图层数据
    if (!jsonData.contains("layers") || !jsonData["layers"].is_array()) {
        // 地图文件中必须有 layers 数组
        spdlog::error("地图文件 '{}' 中缺少或无效的 'layers' 数组。", levelPath);
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

    spdlog::info("关卡加载完成: {}", levelPath);
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
    auto textureId = resolvePath(imagePath);

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
    // TODO
}

void LevelLoader::loadObjectLayer(const nlohmann::json& layerJson, SceneBase& scene)
{
    // TODO
}

std::string LevelLoader::resolvePath(std::string imagePath)
{
    try {
        // 获取地图文件的父目录（相对于可执行文件） "assets/maps/level1.tmj" -> "assets/maps"
        auto mapDir = std::filesystem::path(m_mapPath).parent_path();
        // 合并路径（相对于可执行文件）并返回。
        /* std::filesystem::canonical：解析路径中的当前目录（.）和上级目录（..）导航符，得到一个干净的路径 */
        auto finalPath = std::filesystem::canonical(mapDir / imagePath);
        return finalPath.string();
    } catch (const std::exception& e) {
        spdlog::error("解析路径失败: {}", e.what());
        return imagePath;
    }
}

} // namespace engine::scene
