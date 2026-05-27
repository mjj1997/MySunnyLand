#include "climb_state.h"
#include "../player_component.h"
#include "fall_state.h"
#include "idle_state.h"

#include "../../../engine/component/animation_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"

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

std::unique_ptr<PlayerStateBase> ClimbState::handleInput(engine::core::Context& context)
{
    // 记录攀爬状态下的按键输入标志
    auto inputManager = context.inputManager();
    auto isUp = inputManager.isActionDown("moveUp");
    auto isDown = inputManager.isActionDown("moveDown");
    auto isLeft = inputManager.isActionDown("moveLeft");
    auto isRight = inputManager.isActionDown("moveRight");

    // 根据按键标志，更新速度。按键则移动，不按键则静止。
    auto physicsComponent = m_playerComponent->physicsComponent();
    auto speed = m_playerComponent->climbVelocity();
    glm::vec2 newVelocity{};
    newVelocity.y = isUp ? -speed : isDown ? speed : 0.0f;
    newVelocity.x = isLeft ? -speed : isRight ? speed : 0.0f;
    physicsComponent->setVelocity(newVelocity);

    // 根据按键标志，决定是否播放动画。
    auto animationComponent = m_playerComponent->animationComponent();
    (isUp || isDown || isLeft || isRight)
        ? animationComponent->resumeAnimation() // 有按键则恢复动画播放
        : animationComponent->stopAnimation();  // 无按键则停止动画播放

    return nullptr;
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
