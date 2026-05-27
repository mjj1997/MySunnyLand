#include "climb_state.h"
#include "../player_component.h"
#include "fall_state.h"
#include "idle_state.h"

#include "../../../engine/component/physics_component.h"

#include <spdlog/spdlog.h>

namespace game::component::state {

void ClimbState::enter()
{
    spdlog::debug("进入攀爬状态");
    playAnimation("climb");

    if (auto* physicsComponent = m_playerComponent->physicsComponent(); physicsComponent) {
        physicsComponent->setGravityEnabled(false); // 爬梯时禁用重力
    }
}

std::unique_ptr<PlayerStateBase> ClimbState::update(float deltaTime, engine::core::Context& context)
{
    auto physicsComponent = m_playerComponent->physicsComponent();

    // 如果着地，切换到 Idle 状态
    if (physicsComponent->isCollidedBelow()) {
        return std::make_unique<IdleState>(m_playerComponent);
    }

    // 如果离开梯子区域，切换到 Fall 状态（能走到这里，说明非着地状态）
    if (physicsComponent->isCollidedLadder() == false) {
        return std::make_unique<FallState>(m_playerComponent);
    }

    return nullptr;
}

void ClimbState::exit()
{
    spdlog::debug("退出攀爬状态");

    if (auto* physicsComponent = m_playerComponent->physicsComponent(); physicsComponent) {
        physicsComponent->setGravityEnabled(true); // 爬梯后重新启用重力
    }
}

} // namespace game::component::state
