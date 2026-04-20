#include "level_loader.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

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
    // TODO
}

void LevelLoader::loadTileLayer(const nlohmann::json& layerJson, SceneBase& scene)
{
    // TODO
}

void LevelLoader::loadObjectLayer(const nlohmann::json& layerJson, SceneBase& scene)
{
    // TODO
}

} // namespace engine::scene
