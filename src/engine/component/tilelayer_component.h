#pragma once

#include "component_base.h"

namespace engine::component {

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
