#include "ui_pressed_state.h"
#include "../../core/context.h"
#include "../../input/input_manager.h"
#include "../ui_interactive_element_base.h"
#include "ui_hover_state.h"
#include "ui_normal_state.h"

namespace engine::ui::state {

void UiPressedState::enter()
{
    // 设置 UI 为按下状态的精灵
    m_owner->setCurrentSprite("pressed");
    // 播放按下时的音效
    m_owner->playSound("pressed");
}

std::unique_ptr<UiStateBase> UiPressedState::handleInput(engine::core::Context& context)
{
    const auto& inputManager = context.inputManager();
    const auto& mousePos = inputManager.logicalMousePosition();
    if (inputManager.isActionReleased("mouseLeftClick")) {
        if (!m_owner->isPointInside(mousePos)) {
            // 松开鼠标时，如果鼠标不在 UI 元素内，切换到正常状态
            return std::make_unique<UiNormalState>(m_owner);
        } else { // 松开鼠标时，如果鼠标在 UI 元素内，触发点击事件
            // 触发点击事件
            m_owner->clicked();
            return std::make_unique<UiHoverState>(m_owner);
        }
    }

    return nullptr;
}

} // namespace engine::ui::state
