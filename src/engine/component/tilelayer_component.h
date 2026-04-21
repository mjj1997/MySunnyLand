#pragma once

#include "component_base.h"

namespace engine::component {

/**
 * @brief 定义瓦片的类型，用于游戏逻辑（例如碰撞）。
 */
enum class TileType {
    Empty,  ///< @brief 空白瓦片
    Normal, ///< @brief 普通瓦片
    Solid,  ///< @brief 静止可碰撞瓦片
    // 未来补充其它类型
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

protected:
    // 核心循环方法
    void init() override;
    void update(double deltaTime, engine::core::Context&) override {}
    void render(engine::core::Context& context) override;
};

} // namespace engine::component
