#pragma once

#include "../render/sprite.h"
#include "component_base.h"

#include <glm/vec2.hpp>
#include <vector>

namespace engine::physics {
class PhysicsEngine;
}

namespace engine::component {

/**
 * @brief 定义瓦片的类型，用于游戏逻辑（例如碰撞）。
 */
enum class TileType {
    Empty,     ///< @brief 空白瓦片
    Normal,    ///< @brief 普通瓦片
    Solid,     ///< @brief 静止可碰撞瓦片
    Unisolid,  ///< @brief 单向静止可碰撞瓦片
    Slope_0_1, ///< @brief 斜坡瓦片，高度:左0   右1
    Slope_1_0, ///< @brief 斜坡瓦片，高度:左1   右0
    Slope_0_2, ///< @brief 斜坡瓦片，高度:左0   右1/2
    Slope_2_1, ///< @brief 斜坡瓦片，高度:左1/2 右1
    Slope_1_2, ///< @brief 斜坡瓦片，高度:左1   右1/2
    Slope_2_0, ///< @brief 斜坡瓦片，高度:左1/2 右0
    Hazard,    ///< @brief 危险瓦片（例如火焰、尖刺等）
    Ladder,    ///< @brief 梯子瓦片
    // 未来补充其它类型
};

/**
 * @brief 包含单个瓦片的渲染和逻辑信息。
 */
struct TileInfo
{
    explicit TileInfo(engine::render::Sprite s = engine::render::Sprite(),
                      TileType t = TileType::Empty)
        : sprite{ std::move(s) }
        , type{ t }
    {}

    engine::render::Sprite sprite; ///< @brief 瓦片的视觉表示
    TileType type;                 ///< @brief 瓦片的逻辑类型
};

/**
 * @brief 管理和渲染瓦片地图层。
 *
 * 存储瓦片地图的布局、每个瓦片的精灵信息和类型。
 * 负责在渲染阶段绘制可见的瓦片。
 */
class TileLayerComponent final : public ComponentBase
{
    friend class engine::object::GameObject;

public:
    TileLayerComponent() = default;

    /**
     * @brief 构造函数
     * @param tileSize 单个瓦片尺寸（像素）
     * @param mapSize 地图尺寸（瓦片数）
     * @param tiles 初始化瓦片数据的容器 (会被移动)
     */
    TileLayerComponent(glm::ivec2 tileSize, glm::ivec2 mapSize, std::vector<TileInfo>&& tiles);

    ~TileLayerComponent() override = default;

    // 禁止拷贝和移动
    TileLayerComponent(const TileLayerComponent&) = delete;
    TileLayerComponent& operator=(const TileLayerComponent&) = delete;
    TileLayerComponent(TileLayerComponent&&) = delete;
    TileLayerComponent& operator=(TileLayerComponent&&) = delete;

    /**
     * @brief 根据瓦片坐标获取瓦片信息
     * @param pos 瓦片坐标 (0 <= x < m_mapSize.x, 0 <= y < m_mapSize.y)
     * @return const TileInfo* 指向瓦片信息的指针，如果坐标无效则返回 nullptr
     */
    const TileInfo* tileInfoAt(glm::ivec2 pos) const;

    /**
     * @brief 根据瓦片坐标获取瓦片类型
     * @param pos 瓦片坐标 (0 <= x < m_mapSize.x, 0 <= y < m_mapSize.y)
     * @return TileType 瓦片类型，如果坐标无效则返回 TileType::Empty
     */
    TileType tileTypeAt(glm::ivec2 pos) const;

    /**
      * @brief 根据世界坐标获取瓦片类型
      * @param worldPos 世界坐标(单位：像素)
      * @return TileType 瓦片类型，如果坐标无效或对应空瓦片则返回 TileType::Empty
      */
    TileType tileTypeAtWorldPos(const glm::vec2& worldPos) const;

    // getters and setters
    glm::ivec2 tileSize() const { return m_tileSize; } ///< @brief 获取单个瓦片尺寸
    glm::ivec2 mapSize() const { return m_mapSize; }   ///< @brief 获取地图尺寸
    glm::vec2 worldSize() const                        ///< @brief 获取地图世界尺寸
    {
        return glm::vec2{ m_mapSize.x * m_tileSize.x, m_mapSize.y * m_tileSize.y };
    }
    const std::vector<TileInfo>& tiles() const { return m_tiles; } ///< @brief 获取瓦片容器
    const glm::vec2& offset() const { return m_offset; }           ///< @brief 获取瓦片层的偏移量
    bool isHidden() const { return m_isHidden; } ///< @brief 获取是否隐藏（不渲染）

    ///< @brief 设置瓦片层的偏移量
    void setOffset(glm::vec2 offset) { m_offset = std::move(offset); }
    void setHidden(bool hidden) { m_isHidden = hidden; } ///< @brief 设置是否隐藏（不渲染）
    ///< @brief 设置物理引擎实例指针
    void setPhysicsEngine(engine::physics::PhysicsEngine* engine) { m_physicsEngine = engine; }

protected:
    // 核心循环方法
    void init() override;
    void update(float deltaTime, engine::core::Context&) override {}
    void render(engine::core::Context& context) override;
    void clean() override;

private:
    glm::ivec2 m_tileSize; ///< @brief 单个瓦片尺寸（像素）
    glm::ivec2 m_mapSize;  ///< @brief 地图尺寸（瓦片数）
    ///< @brief 存储所有瓦片信息 (按"行主序"存储, index = y * m_mapSize.x + x)
    std::vector<TileInfo> m_tiles;
    ///< @brief 瓦片层在世界中的偏移量 (瓦片层通常不需要缩放及旋转，因此不引入Transform组件)
    glm::vec2 m_offset{ 0.0F, 0.0F }; // m_offset 最好也保持默认的0，以免增加不必要的复杂性
    bool m_isHidden{ false };         ///< @brief 是否隐藏（不渲染）
    ///< @brief 物理引擎实例指针。clean() 方法中可能需要反注册
    engine::physics::PhysicsEngine* m_physicsEngine{ nullptr };
};

} // namespace engine::component
