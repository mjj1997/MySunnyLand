#pragma once

#include "ui_element_base.h"

#include <string>

namespace engine::render {
class TextRenderer;
}

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

    // --- Setters & Getters ---
    const std::string& text() const { return m_text; }
    const std::string& fontId() const { return m_fontId; }
    int fontSize() const { return m_fontSize; }
    const engine::utils::FColor& textColor() const { return m_textColor; }

    void setText(const std::string& text);     ///< @brief 设置文本内容, 同时更新尺寸
    void setFontId(const std::string& fontId); ///< @brief 设置字体ID, 同时更新尺寸
    void setFontSize(int fontSize);            ///< @brief 设置字体大小, 同时更新尺寸
    void setTextColor(const engine::utils::FColor& textColor);

private:
    ///< @brief 需要文本渲染器，用于获取和更新文本尺寸
    engine::render::TextRenderer& m_textRenderer;

    std::string m_text;   ///< @brief 文本内容
    std::string m_fontId; ///< @brief 字体 ID
    int m_fontSize;       ///< @brief 字体大小
    engine::utils::FColor m_textColor{ 1.0f, 1.0f, 1.0f, 1.0f };

    /* 可添加其他内容，例如边框、底色等 */
};

} // namespace engine::ui
