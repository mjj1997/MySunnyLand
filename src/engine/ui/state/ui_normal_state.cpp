#include "ui_normal_state.h"
#include "../../core/context.h"
#include "../../input/input_manager.h"
#include "../ui_interactive_element_base.h"

namespace engine::ui::state {

void UiNormalState::enter()
{
    // TODO: 设置 UI 为正常状态的精灵
}

std::unique_ptr<UiStateBase> UiNormalState::handleInput(engine::core::Context& context)
{
    auto& inputManager = context.inputManager();
    auto mousePos = inputManager.mousePosition();
    if (m_owner->isPointInside(mousePos)) { // 如果鼠标在 UI 元素内，切换到悬停状态
        // TODO: 切换到悬停状态，并播放悬停时的音乐
    }

    return nullptr;
}

} // namespace engine::ui::state
