#pragma once

#include "ui_element_base.h"

namespace engine::ui {

/**
 * @brief 用于对其他 UI 元素进行分组的容器 UI 元素
 *
 * Panel 通常用于布局和组织。
 * 可以选择是否绘制背景色(纯色)。
 */
class UiPanel final : public UiElementBase
{
public:
    void render(engine::core::Context& context) override;
};

} // namespace engine::ui
