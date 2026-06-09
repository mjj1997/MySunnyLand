#include "ui_panel.h"

namespace engine::ui {

void UiPanel::render(engine::core::Context& context) {}

void UiPanel::setBackgroundColor(const std::optional<engine::utils::FColor>& backgroundColor)
{
    m_backgroundColor = backgroundColor;
}

const std::optional<engine::utils::FColor>& UiPanel::backgroundColor() const
{
    return m_backgroundColor;
}

} // namespace engine::ui
