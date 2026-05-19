#include "idle_state.h"
#include "../player_component.h"
#include "fall_state.h"
#include "jump_state.h"
#include "walk_state.h"

#include "../../../engine/component/physics_component.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"

namespace game::component::state {

void IdleState::enter() {}

std::unique_ptr<PlayerStateBase> IdleState::handleInput(engine::core::Context& context)
{
    auto inputManager = context.inputManager();

    // 如果按下了左、右移动键，切换到 WalkState
    if (inputManager.isActionDown("moveLeft") || inputManager.isActionDown("moveRight")) {
        return std::make_unique<WalkState>(m_playerComponent);
    }

    // 如果按下了跳跃键，切换到 JumpState
    if (inputManager.isActionPressed("jump")) {
        return std::make_unique<JumpState>(m_playerComponent);
    }

    return nullptr;
}

std::unique_ptr<PlayerStateBase> IdleState::update(float deltaTime, engine::core::Context& context)
{
    // 应用摩擦力(水平方向上)
    auto physicsComponent = m_playerComponent->physicsComponent();
    auto frictionFactor = m_playerComponent->frictionFactor();
    auto newVeclocity = glm::vec2{ frictionFactor * physicsComponent->velocity().x,
                                   physicsComponent->velocity().y };
    physicsComponent->setVelocity(newVeclocity);

    // 如果下方没有碰撞，切换到 FallState
    if (!physicsComponent->isCollidedBelow()) {
        return std::make_unique<FallState>(m_playerComponent);
    }

    return nullptr;
}

void IdleState::exit() {}

} // namespace game::component::state
