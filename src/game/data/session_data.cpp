#include "session_data.h"

#include <glm/common.hpp>
#include <spdlog/spdlog.h>

namespace game::data {

void SessionData::setCurrentHealth(int health)
{
    // 将生命值限制在 0 到 m_maxHealth 之间
    m_currentHealth = glm::clamp(health, 0, m_maxHealth);
}

void SessionData::setMaxHealth(int maxHealth)
{
    if (maxHealth > 0) {
        m_maxHealth = maxHealth;
        // 确保当前生命值不超过最大生命值
        setCurrentHealth(m_currentHealth);
    } else {
        spdlog::warn("尝试将最大生命值设置为非正数: {}", maxHealth);
    }
}

void SessionData::addScore(int scoreToAdd)
{
    m_currentScore += scoreToAdd;
    // 如果当前得分超过最高分，更新最高分
    setHighestScore(glm::max(m_highestScore, m_currentScore));
}

} // namespace game::data
