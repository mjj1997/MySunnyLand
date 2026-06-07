#include "ui_element_base.h"

namespace engine::ui {

UiElementBase::UiElementBase(const glm::vec2& localPosition, const glm::vec2& size)
    : m_localPosition{ localPosition }
    , m_size{ size }
{}

} // namespace engine::ui
