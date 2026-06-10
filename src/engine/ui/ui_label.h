#pragma once

#include "ui_element_base.h"

namespace engine::ui {

/**
 * @brief 用于显示和管理 UI 中的文本标签的 UI 元素
 *
 * Label 添加了渲染文本的功能。
 * 可以设置文本内容、字体 ID、字体大小和文本颜色等属性。
 * 
 * @note 需要一个文本渲染器来获取和更新文本尺寸。
 */
class UiLabel final : public UiElementBase
{
public:
    void render(engine::core::Context& context) override;
};

} // namespace engine::ui
