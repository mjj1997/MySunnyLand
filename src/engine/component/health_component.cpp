#include "health_component.h"
#include "../object/game_object.h"

#include <glm/common.hpp>
#include <spdlog/spdlog.h>

namespace engine::component {

HealthComponent::HealthComponent(int maxHealth, float invincibilityDuration)
    : m_maxHealth{ glm::max(1, maxHealth) } // 确保最大生命值至少为 1
    , m_currentHealth{ m_maxHealth }        // 初始化当前生命值为最大生命值
    , m_invincibilityDuration{ invincibilityDuration }
{}

bool HealthComponent::takeDamage(int damageAmount)
{
    if (damageAmount <= 0 || !isAlive()) {
        return false; // 无效伤害或对象已死亡，不造成伤害
    }

    if (m_isInvincible) {
        spdlog::debug("游戏对象 {} 处于无敌状态，免疫了 {} 伤害。",
                      m_owner != nullptr ? m_owner->name() : "未知",
                      damageAmount);
        return false; // 无敌状态，不造成伤害
    }

    // --- 确实造成伤害了 ---
    auto health = m_currentHealth - damageAmount;
    setCurrentHealth(health);
    // 如果对象存活且设置了无敌时长，则触发无敌帧
    if (isAlive() && m_invincibilityDuration > 0.0F) {
        setInvincible(m_invincibilityDuration);
    }
    spdlog::debug("游戏对象 {} 收到了 {} 点伤害，当前生命值：{}/{}",
                  m_owner != nullptr ? m_owner->name() : "未知",
                  damageAmount,
                  m_currentHealth,
                  m_maxHealth);

    return true;
}

void HealthComponent::heal(int healAmount)
{
    if (healAmount <= 0 || !isAlive()) {
        return; // 无效治疗或对象已死亡，不造成治疗
    }

    auto health = m_currentHealth + healAmount;
    setCurrentHealth(health);
    spdlog::debug("游戏对象 {} 治疗了 {} 点生命值，当前生命值：{}/{}",
                  m_owner != nullptr ? m_owner->name() : "未知",
                  healAmount,
                  m_currentHealth,
                  m_maxHealth);
}

void HealthComponent::setCurrentHealth(int currentHealth)
{
    // 确保生命值在 0 到最大生命值之间
    m_currentHealth = glm::clamp(currentHealth, 0, m_maxHealth);
}

void HealthComponent::setMaxHealth(int maxHealth)
{
    m_maxHealth = glm::max(1, maxHealth);                     // 确保最大生命值至少为 1
    m_currentHealth = glm::min(m_currentHealth, m_maxHealth); // 确保当前生命值不超过最大生命值
}

void HealthComponent::setInvincible(float duration)
{
    if (duration > 0.0F) {
        m_isInvincible = true;
        m_invincibilityTimer = duration;
        spdlog::debug("游戏对象 {} 进入无敌状态，持续 {} 秒",
                      m_owner != nullptr ? m_owner->name() : "未知",
                      duration);
    } else {
        // 如果持续时间 <= 0, 则立即退出无敌状态。即手动取消无敌。
        m_isInvincible = false;
        m_invincibilityTimer = 0.0F;
        spdlog::debug("游戏对象 {} 的无敌状态被手动移除。",
                      m_owner != nullptr ? m_owner->name() : "未知");
    }
}

void HealthComponent::update(float deltaTime, engine::core::Context& /*context*/)
{
    // 更新无敌状态计时器
    if (m_isInvincible) {
        m_invincibilityTimer -= deltaTime;
        if (m_invincibilityTimer <= 0.0F) {
            m_isInvincible = false;
            m_invincibilityTimer = 0.0F;
        }
    }
}

} // namespace engine::component
