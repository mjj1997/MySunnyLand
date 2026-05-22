#include "jump_state.h"
#include "../player_component.h"
#include "fall_state.h"

#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"

#include <glm/common.hpp>
#include <spdlog/spdlog.h>

namespace game::component::state {

void JumpState::enter()
{
    playAnimation("jump"); // 播放跳跃动画

    auto physicsComponent = m_playerComponent->physicsComponent();
    auto newVelocity = glm::vec2{ physicsComponent->velocity().x, -m_playerComponent->jumpForce() };
    physicsComponent->setVelocity(newVelocity);

    spdlog::debug("PlayerComponent 进入 JumpState， 设置初始垂直速度为：{}",
                  physicsComponent->velocity().y);
}

std::unique_ptr<PlayerStateBase> JumpState::handleInput(engine::core::Context& context)
{
    auto inputManager = context.inputManager();
    auto physicsComponent = m_playerComponent->physicsComponent();
    auto spriteComponent = m_playerComponent->spriteComponent();

    // 跳跃状态下可以左右移动
    if (inputManager.isActionDown("moveLeft")) {
        // 如果当前速度是向右的，则先减速到0 (增强操控手感)
        if (physicsComponent->velocity().x > 0.0f) {
            glm::vec2 newVelocity{ 0.0f, physicsComponent->velocity().y };
            physicsComponent->setVelocity(newVelocity);
        }

        // 添加向左的水平力
        glm::vec2 moveLeftForce{ -m_playerComponent->moveForce(), 0.0f };
        physicsComponent->addForce(moveLeftForce);
        spriteComponent->setFlipped(true); // 向左移动时翻转
    } else if (inputManager.isActionDown("moveRight")) {
        // 如果当前速度是向左的，则先减速到0
        if (physicsComponent->velocity().x < 0.0f) {
            glm::vec2 newVelocity{ 0.0f, physicsComponent->velocity().y };
            physicsComponent->setVelocity(newVelocity);
        }

        // 添加向右的水平力
        glm::vec2 moveRightForce{ m_playerComponent->moveForce(), 0.0f };
        physicsComponent->addForce(moveRightForce);
        spriteComponent->setFlipped(false); // 向右移动时不翻转
    }

    return nullptr;
}

std::unique_ptr<PlayerStateBase> JumpState::update(float deltaTime, engine::core::Context& context)
{
    /** 限制最大速度（水平方向）
     *  因为向上跳的速度不会超过最大速度，所以只需要限制左右移动时 x 轴速度
     */
    auto physicsComponent = m_playerComponent->physicsComponent();
    auto maxSpeed = m_playerComponent->maxSpeed();
    auto velocityX = glm::clamp(physicsComponent->velocity().x, -maxSpeed, maxSpeed);
    glm::vec2 newVelocity{ velocityX, physicsComponent->velocity().y };
    physicsComponent->setVelocity(newVelocity);

    // 如果 y 方向速度大于0，即向下移动，切换到 FallState
    if (physicsComponent->velocity().y > 0.0f) {
        return std::make_unique<FallState>(m_playerComponent);
    }

    return nullptr;
}

void JumpState::exit() {}

} // namespace game::component::state
