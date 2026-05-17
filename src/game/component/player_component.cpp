#include "player_component.h"
#include "state/player_state_base.h"

#include <spdlog/spdlog.h>

namespace game::component {

void PlayerComponent::setState(std::unique_ptr<state::PlayerStateBase> newState)
{
    if (!newState) {
        spdlog::warn("正在尝试设置玩家状态为空！");
        return;
    }

    if (m_currentState) {
        m_currentState->exit();
    }

    m_currentState = std::move(newState);
    spdlog::debug("PlayerComponent 正在切换到状态：{}。", typeid(*m_currentState).name());
    m_currentState->enter();
}

void PlayerComponent::init()
{
}

void PlayerComponent::handleInput(engine::core::Context& context)
{
}

void PlayerComponent::update(float deltaTime, engine::core::Context& context)
{
}

} // namespace game::component
