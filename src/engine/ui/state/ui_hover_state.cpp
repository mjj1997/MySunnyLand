#include "ui_hover_state.h"
#include "../../core/context.h"
#include "../../input/input_manager.h"
#include "../ui_interactive_element_base.h"
#include "ui_normal_state.h"
#include "ui_pressed_state.h"

namespace engine::ui::state {

void UiHoverState::enter()
{
    // TODO: 设置 UI 为悬停状态的精灵
}

std::unique_ptr<UiStateBase> UiHoverState::handleInput(engine::core::Context& context)
{
    auto& inputManager = context.inputManager();
    auto mousePos = inputManager.mousePosition();
    if (!m_owner->isPointInside(mousePos)) { // 如果鼠标不在 UI 元素内，切换到正常状态
        return std::make_unique<UiNormalState>(m_owner);
    }

    if (inputManager.isActionPressed("mouseLeftClick")) { // 如果鼠标点击了左键，切换到按下状态
        return std::make_unique<UiPressedState>(m_owner);
    }

    return nullptr;
}

} // namespace engine::ui::state
