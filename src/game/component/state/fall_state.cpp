#include "fall_state.h"
#include "../player_component.h"
#include "climb_state.h"
#include "idle_state.h"
#include "walk_state.h"

#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"

#include <glm/common.hpp>

namespace game::component::state {

void FallState::enter()
{
    playAnimation("fall"); // 播放下落动画
}

std::unique_ptr<PlayerStateBase> FallState::handleInput(engine::core::Context& context)
{
    auto inputManager = context.inputManager();
    auto physicsComponent = m_playerComponent->physicsComponent();
    auto spriteComponent = m_playerComponent->spriteComponent();

    // 如果按下了"moveUp"或"moveDown"键，且与梯子重合，切换到 ClimbState
    if ((inputManager.isActionDown("moveUp") || inputManager.isActionDown("moveDown"))
        && physicsComponent->isCollidedLadder()) {
        return std::make_unique<ClimbState>(m_playerComponent);
    }

    // 下落状态下可以左右移动
    if (inputManager.isActionDown("moveLeft")) {
        // 如果当前速度是向右的，则先减速到0 (增强操控手感)
        if (physicsComponent->velocity().x > 0.0F) {
            const glm::vec2 newVelocity{ 0.0F, physicsComponent->velocity().y };
            physicsComponent->setVelocity(newVelocity);
        }

        // 添加向左的水平力
        const glm::vec2 moveLeftForce{ -m_playerComponent->moveForce(), 0.0F };
        physicsComponent->addForce(moveLeftForce);
        spriteComponent->setFlipped(true); // 向左移动时翻转
    } else if (inputManager.isActionDown("moveRight")) {
        // 如果当前速度是向左的，则先减速到0
        if (physicsComponent->velocity().x < 0.0F) {
            const glm::vec2 newVelocity{ 0.0F, physicsComponent->velocity().y };
            physicsComponent->setVelocity(newVelocity);
        }

        // 添加向右的水平力
        const glm::vec2 moveRightForce{ m_playerComponent->moveForce(), 0.0F };
        physicsComponent->addForce(moveRightForce);
        spriteComponent->setFlipped(false); // 向右移动时不翻转
    }

    return nullptr;
}

std::unique_ptr<PlayerStateBase> FallState::update(float /*deltaTime*/,
                                                   engine::core::Context& /*context*/)
{
    // 限制最大速度（水平方向）
    auto physicsComponent = m_playerComponent->physicsComponent();
    auto maxSpeed = m_playerComponent->maxSpeed();
    auto velocityX = glm::clamp(physicsComponent->velocity().x, -maxSpeed, maxSpeed);
    const glm::vec2 newVelocity{ velocityX, physicsComponent->velocity().y };
    physicsComponent->setVelocity(newVelocity);

    // 如果下方有碰撞，就根据水平速度来决定切换到 IdleState 或 WalkState
    if (physicsComponent->isCollidedBelow()) {
        if (glm::abs(physicsComponent->velocity().x) < 1.0F) {
            return std::make_unique<IdleState>(m_playerComponent);
        } else {
            return std::make_unique<WalkState>(m_playerComponent);
        }
    }

    return nullptr;
}

void FallState::exit() {}

} // namespace game::component::state
