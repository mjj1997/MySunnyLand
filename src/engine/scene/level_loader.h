#pragma once

#include <glm/vec2.hpp>
#include <nlohmann/json_fwd.hpp>

#include <map>
#include <string>

namespace engine::component {
struct TileInfo;
}

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

    /**
     * @brief 解析图片路径，合并文件路径和相对路径。例如：
     * 1. 文件路径："assets/maps/level1.tmj"
     * 2. 相对路径："../textures/Layers/back.png"
     * 3. 最终路径："assets/textures/Layers/back.png"
     * @param relativePath 相对路径（相对于文件）
     * @param filePath 文件路径
     * @return std::string 解析后的完整路径。
     */
    std::string resolvePath(const std::string& relativePath, const std::string& filePath);

    engine::component::TileInfo tileInfoByGid(int gid);
    void loadTileset(const std::string& tilesetPath, int firstGid);

    ///< @brief 地图路径（拼接路径时需要）
    std::string m_mapPath;
    glm::ivec2 m_mapSize;
    glm::ivec2 m_tileSize;
    std::map<int, nlohmann::json> m_tilesets;
};

} // namespace engine::scene
