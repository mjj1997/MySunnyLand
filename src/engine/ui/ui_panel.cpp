#include "ui_panel.h"
#include "../core/context.h"
#include "../render/renderer.h"

#include <spdlog/spdlog.h>

namespace engine::ui {

UiPanel::UiPanel(const glm::vec2& localPosition,
                 const glm::vec2& size,
                 const std::optional<engine::utils::FColor>& backgroundColor)
    : UiElementBase{ localPosition, size }
    , m_backgroundColor{ backgroundColor }
{
    spdlog::trace("UiPanel 构造完成");
}

void UiPanel::render(engine::core::Context& context)
{
    if (!m_isVisible) {
        return;
    }

    if (m_backgroundColor.has_value()) {
        context.renderer().drawUiFilledRect(bounds(), m_backgroundColor.value());
    }

    UiElementBase::render(context); // 调用基类方法渲染子元素
}

void UiPanel::setBackgroundColor(const std::optional<engine::utils::FColor>& backgroundColor)
{
    m_backgroundColor = backgroundColor;
}

const std::optional<engine::utils::FColor>& UiPanel::backgroundColor() const
{
    return m_backgroundColor;
}

} // namespace engine::ui
