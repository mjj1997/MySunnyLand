#include "idle_state.h"
#include "../player_component.h"
#include "climb_state.h"
#include "fall_state.h"
#include "jump_state.h"
#include "walk_state.h"

#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/transform_component.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"

namespace game::component::state {

void IdleState::enter()
{
    playAnimation("idle"); // 播放待机动画
}

std::unique_ptr<PlayerStateBase> IdleState::handleInput(engine::core::Context& context)
{
    const auto& inputManager = context.inputManager();
    auto* physicsComponent = m_playerComponent->physicsComponent();

    // 如果按下了"moveUp"键，且与梯子重合，切换到 ClimbState
    if (inputManager.isActionDown("moveUp") && physicsComponent->isCollidedLadder()) {
        return std::make_unique<ClimbState>(m_playerComponent);
    }

    // 如果按下了"moveDown"键，且位于梯子顶端，切换到 ClimbState
    if (inputManager.isActionDown("moveDown") && physicsComponent->isOnLadderTop()) {
        // 需要向下移动一点，确保下一帧能与梯子碰撞（否则会切换回FallState）
        m_playerComponent->transformComponent()->translate(glm::vec2{ 0.0F, 2.0F });
        return std::make_unique<ClimbState>(m_playerComponent);
    }

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

    // 如果离地，切换到 FallState
    if (!m_playerComponent->isOnGround()) {
        return std::make_unique<FallState>(m_playerComponent);
    }

    return nullptr;
}

void IdleState::exit() {}

} // namespace game::component::state
