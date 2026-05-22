#include "hurt_state.h"
#include "../player_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"

#include <glm/common.hpp>

namespace game::component::state {

void HurtState::enter()
{
    // 播放受伤动画
    playAnimation("hurt");

    // --- 造成击退效果 ---
    auto physicsComponent = m_playerComponent->physicsComponent();
    auto spriteComponent = m_playerComponent->spriteComponent();
    auto knockbackVelocity = glm::vec2{ -100.0f, -150.0f }; // 默认往左上方击退
    // 根据当前精灵的朝向状态，判断是否需要往右上方击退
    if (spriteComponent->isFlipped()) {
        knockbackVelocity.x = -knockbackVelocity.x; // 击退方向变为向右
    }
    // 设置击退速度
    physicsComponent->setVelocity(knockbackVelocity);
}

std::unique_ptr<PlayerStateBase> HurtState::handleInput(engine::core::Context& context)
{
    // 受伤硬直期间，不能进行任何操控
    return nullptr;
}

std::unique_ptr<PlayerStateBase> HurtState::update(float deltaTime, engine::core::Context& context)
{
    m_stunnedTimer += deltaTime;
    return nullptr;
}

void HurtState::exit() {}

} // namespace game::component::state
