#include "ui_normal_state.h"
#include "../../core/context.h"
#include "../../input/input_manager.h"
#include "../ui_interactive_element_base.h"
#include "ui_hover_state.h"

namespace engine::ui::state {

void UiNormalState::enter()
{
    // 设置 UI 为正常状态的精灵
    m_owner->setCurrentSprite("normal");
}

std::unique_ptr<UiStateBase> UiNormalState::handleInput(engine::core::Context& context)
{
    const auto& inputManager = context.inputManager();
    const auto& mousePos = inputManager.logicalMousePosition();
    if (m_owner->isPointInside(mousePos)) { // 如果鼠标在 UI 元素内，切换到悬停状态
        // 播放悬停时的音效
        m_owner->playSound("hover");
        return std::make_unique<UiHoverState>(m_owner);
    }

    return nullptr;
}

} // namespace engine::ui::state
