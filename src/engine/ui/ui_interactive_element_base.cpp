#include "ui_interactive_element_base.h"

#include <spdlog/spdlog.h>

namespace engine::ui {

bool UiInteractiveElementBase::handleInput(engine::core::Context& context)
{
    return false;
}

void UiInteractiveElementBase::render(engine::core::Context& context) {}

void UiInteractiveElementBase::setCurrentState(std::unique_ptr<engine::ui::state::UiStateBase> state)
{
    if (state == nullptr) {
        spdlog::error("尝试设置空的状态！");
        return;
    }

    m_currentState = std::move(state);
    m_currentState->enter();
}

} // namespace engine::ui
