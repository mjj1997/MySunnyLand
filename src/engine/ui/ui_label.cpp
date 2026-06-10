#include "ui_label.h"
#include "../render/text_renderer.h"

namespace engine::ui {

void UiLabel::render(engine::core::Context& context) {}

void UiLabel::setText(const std::string& text)
{
    m_text = text;
    m_size = m_textRenderer.getTextSize(m_text, m_fontId, m_fontSize);
}

void UiLabel::setFontId(const std::string& fontId)
{
    m_fontId = fontId;
    m_size = m_textRenderer.getTextSize(m_text, m_fontId, m_fontSize);
}

void UiLabel::setFontSize(int fontSize)
{
    m_fontSize = fontSize;
    m_size = m_textRenderer.getTextSize(m_text, m_fontId, m_fontSize);
}

void UiLabel::setTextColor(const engine::utils::FColor& textColor)
{
    m_textColor = textColor;
    /* 颜色变化, 不需要更新尺寸 */
}

} // namespace engine::ui
