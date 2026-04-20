#pragma once

#include <nlohmann/json_fwd.hpp>

#include <string>

namespace engine::scene {
class SceneBase;

class LevelLoader final
{
public:
    LevelLoader() = default;

    /**
     * @brief 加载关卡数据到指定的 Scene 对象中。
     * @param mapPath Tiled JSON 地图文件的路径。
     * @param scene 要加载数据的目标 Scene 对象。
     * @return bool 是否加载成功。
     */
    bool loadLevel(const std::string& mapPath, SceneBase& scene);

private:
    ///< @brief 加载图片图层
    void loadImageLayer(const nlohmann::json& layerJson, SceneBase& scene);
    ///< @brief 加载瓦片图层
    void loadTileLayer(const nlohmann::json& layerJson, SceneBase& scene);
    ///< @brief 加载对象图层
    void loadObjectLayer(const nlohmann::json& layerJson, SceneBase& scene);

    ///< @brief 地图路径（拼接路径时需要）
    std::string m_mapPath;
};

} // namespace engine::scene
