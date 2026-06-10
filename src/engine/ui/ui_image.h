#pragma once

#include "ui_element_base.h"

namespace engine::ui {

/**
 * @brief 用于显示纹理或部分纹理的 UI 元素
 *
 * Image 添加了渲染图像的功能。
 */
class UiImage final : public UiElementBase
{
public:
    void render(engine::core::Context& context) override;
};

} // namespace engine::ui
