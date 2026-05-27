#include "climb_state.h"
#include "../player_component.h"

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

void ClimbState::exit()
{
    spdlog::debug("退出攀爬状态");

    if (auto* physicsComponent = m_playerComponent->physicsComponent(); physicsComponent) {
        physicsComponent->setGravityEnabled(true); // 爬梯后重新启用重力
    }
}

} // namespace game::component::state
