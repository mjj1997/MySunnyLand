#include "hurt_state.h"
#include "../player_component.h"
#include "fall_state.h"
#include "idle_state.h"
#include "walk_state.h"

#include "../../../engine/component/audio_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"

#include <glm/common.hpp>

namespace game::component::state {

void HurtState::enter()
{
    // 播放受伤动画
    playAnimation("hurt");

    // --- 造成击退效果 ---
    auto* physicsComponent = m_playerComponent->physicsComponent();
    auto* spriteComponent = m_playerComponent->spriteComponent();
    glm::vec2 knockbackVelocity{ -100.0F, -150.0F }; // 默认往左上方击退
    // 根据当前精灵的朝向状态，判断是否需要往右上方击退
    if (spriteComponent->isFlipped()) {
        knockbackVelocity.x = -knockbackVelocity.x; // 击退方向变为向右
    }
    // 设置击退速度
    physicsComponent->setVelocity(knockbackVelocity);

    if (auto* audioComponent = m_playerComponent->audioComponent(); audioComponent) {
        audioComponent->playSound("hurt"); // 播放受伤音效
    }
}

std::unique_ptr<PlayerStateBase> HurtState::handleInput(engine::core::Context& /*context*/)
{
    // 受伤硬直期间，不能进行任何操控
    return nullptr;
}

std::unique_ptr<PlayerStateBase> HurtState::update(float deltaTime,
                                                   engine::core::Context& /*context*/)
{
    m_stunnedTimer += deltaTime;

    // --- 两种情况离开受伤（硬直）状态 ---
    auto* physicsComponent = m_playerComponent->physicsComponent();
    // 1. 落地
    if (physicsComponent->isCollidedBelow()) {
        if (glm::abs(physicsComponent->velocity().x) < 1.0F) {
            return std::make_unique<IdleState>(m_playerComponent);
        } else {
            return std::make_unique<WalkState>(m_playerComponent);
        }
    }
    // 2. 硬直时间结束（能走到这里，说明没有落地，直接切换到 FallState）
    if (m_stunnedTimer > m_playerComponent->stunnedDuration()) {
        m_stunnedTimer = 0.0F; // 重置硬直计时器
        return std::make_unique<FallState>(m_playerComponent);
    }

    return nullptr;
}

void HurtState::exit() {}

} // namespace game::component::state
