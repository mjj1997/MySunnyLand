#include "health_component.h"
#include "../object/game_object.h"

#include <glm/common.hpp>
#include <spdlog/spdlog.h>

namespace engine::component {

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
    if (duration > 0.0f) {
        m_isInvincible = true;
        m_invincibilityTimer = duration;
        spdlog::debug("游戏对象 {} 进入无敌状态，持续 {} 秒",
                      m_owner ? m_owner->name() : "未知",
                      duration);
    } else {
        // 如果持续时间 <= 0, 则立即退出无敌状态。即手动取消无敌。
        m_isInvincible = false;
        m_invincibilityTimer = 0.0f;
        spdlog::debug("游戏对象 {} 的无敌状态被手动移除。", m_owner ? m_owner->name() : "未知");
    }
}

void HealthComponent::update(float deltaTime, engine::core::Context& context) {}

} // namespace engine::component
