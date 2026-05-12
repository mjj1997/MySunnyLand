#pragma once

#include "../utils/math.h"

#include <glm/vec2.hpp>
#include <nlohmann/json_fwd.hpp>

#include <map>
#include <string>

namespace engine::component {
struct TileInfo;
enum class TileType;
} // namespace engine::component

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

    void loadTileset(const std::string& tileSetPath, int firstGid);
    engine::component::TileInfo tileInfoByGid(int gid);
    engine::component::TileType tileTypeById(const nlohmann::json& tileSetJson, int localId);
    engine::component::TileType getTileType(const nlohmann::json& tileJson);

    /**
     * @brief 获取瓦片属性
     * @tparam T 属性类型
     * @param tileJson 瓦片json数据
     * @param propertyName 属性名称
     * @return 属性值，如果属性不存在则返回 std::nullopt
     */
    template<typename T>
    std::optional<T> getTileProperty(const nlohmann::json& tileJson, const std::string& propertyName)
    {
        if (!tileJson.contains("properties")) {
            return std::nullopt;
        }

        for (const auto& property : tileJson["properties"]) {
            if (property.contains("name") && property["name"] == propertyName) {
                if (property.contains("value")) {
                    return property["value"].get<T>();
                }
            }
        }

        return std::nullopt;
    }

    /**
     * @brief 获取瓦片碰撞器矩形
     * @param tileJson 瓦片json数据
     * @return 碰撞器矩形，如果碰撞器不存在则返回 std::nullopt
     */
    std::optional<engine::utils::Rect> getColliderRect(const nlohmann::json& tileJson);

    /**
     * @brief 根据全局 ID 获取瓦片json对象 (用于对象层获取瓦片信息)
     * @param gid 全局 ID
     * @return 瓦片json对象
     */
    std::optional<nlohmann::json> getTileJsonByGid(int gid) const;

    ///< @brief 地图路径（拼接路径时需要）
    std::string m_mapPath;
    glm::ivec2 m_mapSize;
    glm::ivec2 m_tileSize;
    std::map<int, nlohmann::json> m_tileSets;
};

} // namespace engine::scene
