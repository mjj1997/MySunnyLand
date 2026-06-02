#include "player_component.h"
#include "state/dead_state.h"
#include "state/hurt_state.h"
#include "state/idle_state.h"

#include "../../engine/component/animation_component.h"
#include "../../engine/component/audio_component.h"
#include "../../engine/component/health_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/object/game_object.h"

#include <spdlog/spdlog.h>

namespace game::component {

bool PlayerComponent::takeDamage(int damageAmount)
{
    if (!m_isAlive || !m_healthComponent || damageAmount <= 0) {
        spdlog::warn("玩家已死亡或缺少必要组件，并未造成伤害。");
        return false;
    }

    bool success{ m_healthComponent->takeDamage(damageAmount) };
    if (!success) {
        return false;
    }

    // --- 成功造成伤害了，根据是否存活决定状态切换 ---
    if (m_healthComponent->isAlive()) {
        spdlog::debug("玩家受到了 {} 点伤害，当前生命值: {}/{}。",
                      damageAmount,
                      m_healthComponent->currentHealth(),
                      m_healthComponent->maxHealth());
        // 切换到受伤状态
        setState(std::make_unique<state::HurtState>(this));
    } else {
        spdlog::debug("玩家死亡。");
        m_isAlive = false;
        // 切换到死亡状态
        setState(std::make_unique<state::DeadState>(this));
    }

    return true;
}

bool PlayerComponent::isOnGround() const
{
    return m_coyoteTimer <= m_coyoteTime || m_physicsComponent->isCollidedBelow();
}

void PlayerComponent::setState(std::unique_ptr<state::PlayerStateBase> newState)
{
    if (!newState) {
        spdlog::warn("正在尝试设置玩家状态为空！");
        return;
    }

    if (m_currentState) {
        m_currentState->exit();
    }

    m_currentState = std::move(newState);
    spdlog::debug("PlayerComponent 正在切换到状态：{}。", typeid(*m_currentState).name());
    m_currentState->enter();
}

void PlayerComponent::init()
{
    if (!m_owner) {
        spdlog::error("PlayerComponent 没有所属游戏对象!");
        return;
    }

    // 获取必要的组件
    m_transformComponent = m_owner->getComponent<engine::component::TransformComponent>();
    m_physicsComponent = m_owner->getComponent<engine::component::PhysicsComponent>();
    m_spriteComponent = m_owner->getComponent<engine::component::SpriteComponent>();
    m_animationComponent = m_owner->getComponent<engine::component::AnimationComponent>();
    m_healthComponent = m_owner->getComponent<engine::component::HealthComponent>();
    m_audioComponent = m_owner->getComponent<engine::component::AudioComponent>();

    // 检查必要组件是否存在
    if (!m_transformComponent || !m_physicsComponent || !m_spriteComponent || !m_animationComponent
        || !m_healthComponent || !m_audioComponent) {
        spdlog::error("Player 对象缺少必要组件！");
        return;
    }

    // 初始化状态机
    m_currentState = std::make_unique<state::IdleState>(this);
    if (m_currentState) {
        setState(std::move(m_currentState));
    } else {
        spdlog::error("初始化玩家状态失败（make_unique 返回空指针）！");
    }

    spdlog::debug("PlayerComponent 初始化完成。");
}

void PlayerComponent::handleInput(engine::core::Context& context)
{
    if (!m_currentState) {
        return;
    }

    auto nextState = m_currentState->handleInput(context);
    if (nextState) {
        setState(std::move(nextState));
    }
}

void PlayerComponent::update(float deltaTime, engine::core::Context& context)
{
    if (!m_currentState) {
        return;
    }

    // 一旦离地，开始计时土狼时间
    if (!m_physicsComponent->isCollidedBelow()) {
        m_coyoteTimer += deltaTime;
    } else { // 一旦落地，重置土狼时间
        m_coyoteTimer = 0.0f;
    }

    // 如果处于无敌状态，就进行闪烁
    if (m_healthComponent->isInvincible()) {
        m_flashTimer += deltaTime;
        if (m_flashTimer >= 2 * m_flashInterval) {
            m_flashTimer = 0.0f;
        }
        // 一半时间可见，一半时间不可见
        if (m_flashTimer < m_flashInterval) {
            m_spriteComponent->setHidden(true);
        } else {
            m_spriteComponent->setHidden(false);
        }
    } else if (m_spriteComponent->isHidden()) { // 非无敌状态时，确保玩家可见
        m_spriteComponent->setHidden(false);
    }

    // 更新当前状态
    auto nextState = m_currentState->update(deltaTime, context);
    if (nextState) {
        setState(std::move(nextState));
    }
}

} // namespace game::component
