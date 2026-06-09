#include "ui_panel.h"

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


void UiPanel::setBackgroundColor(const std::optional<engine::utils::FColor>& backgroundColor)
{
    m_backgroundColor = backgroundColor;
}

const std::optional<engine::utils::FColor>& UiPanel::backgroundColor() const
{
    return m_backgroundColor;
}

} // namespace engine::ui
