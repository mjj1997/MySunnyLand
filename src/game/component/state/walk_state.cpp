#include "walk_state.h"
#include "../player_component.h"
#include "climb_state.h"
#include "fall_state.h"
#include "idle_state.h"
#include "jump_state.h"

#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"

#include <glm/common.hpp>

namespace game::component::state {

void WalkState::enter()
{
    playAnimation("walk"); // 播放行走动画
}

std::unique_ptr<PlayerStateBase> WalkState::handleInput(engine::core::Context& context)
{
    auto inputManager = context.inputManager();
    auto physicsComponent = m_playerComponent->physicsComponent();
    auto spriteComponent = m_playerComponent->spriteComponent();

    // 如果按下了"moveUp"键，且与梯子重合，切换到 ClimbState
    if (inputManager.isActionDown("moveUp") && physicsComponent->isCollidedLadder()) {
        return std::make_unique<ClimbState>(m_playerComponent);
    }

    // 如果按下“jump”则切换到 JumpState
    if (inputManager.isActionPressed("jump")) {
        return std::make_unique<JumpState>(m_playerComponent);
    }

    // 步行状态可以左右移动
    if (inputManager.isActionDown("moveLeft")) {
        // 如果当前速度是向右的，则先减速到0 (增强操控手感)
        if (physicsComponent->velocity().x > 0.0F) {
            glm::vec2 newVelocity{ 0.0F, physicsComponent->velocity().y };
            physicsComponent->setVelocity(newVelocity);
        }

        // 添加向左的水平力
        glm::vec2 moveLeftForce{ -m_playerComponent->moveForce(), 0.0F };
        physicsComponent->addForce(moveLeftForce);
        spriteComponent->setFlipped(true); // 向左移动时翻转
    } else if (inputManager.isActionDown("moveRight")) {
        // 如果当前速度是向左的，则先减速到0
        if (physicsComponent->velocity().x < 0.0F) {
            glm::vec2 newVelocity{ 0.0F, physicsComponent->velocity().y };
            physicsComponent->setVelocity(newVelocity);
        }

        // 添加向右的水平力
        glm::vec2 moveRightForce{ m_playerComponent->moveForce(), 0.0F };
        physicsComponent->addForce(moveRightForce);
        spriteComponent->setFlipped(false); // 向右移动时不翻转
    } else {
        // 如果没有按下左右移动键，则切换到 IdleState
        return std::make_unique<IdleState>(m_playerComponent);
    }

    return nullptr;
}

std::unique_ptr<PlayerStateBase> WalkState::update(float deltaTime, engine::core::Context& context)
{
    /** 限制最大速度
     *  因为行走状态只涉及左右移动，所以只需要限制 x 轴速度
     */
    auto physicsComponent = m_playerComponent->physicsComponent();
    auto maxSpeed = m_playerComponent->maxSpeed();
    auto velocityX = glm::clamp(physicsComponent->velocity().x, -maxSpeed, maxSpeed);
    glm::vec2 newVelocity{ velocityX, physicsComponent->velocity().y };
    physicsComponent->setVelocity(newVelocity);

    // 如果离地，切换到 FallState
    if (!m_playerComponent->isOnGround()) {
        return std::make_unique<FallState>(m_playerComponent);
    }

    return nullptr;
}

void WalkState::exit() {}

} // namespace game::component::state
