#include "jump_behavior.h"

#include <spdlog/spdlog.h>

namespace game::component::ai {

JumpBehavior::JumpBehavior(float minX, float maxX, glm::vec2 jumpVelocity, float jumpInterval)
    : m_patrolMinX{ minX }
    , m_patrolMaxX{ maxX }
    , m_jumpVelocity{ jumpVelocity }
    , m_jumpInterval{ jumpInterval }
{
    if (m_patrolMinX > m_patrolMaxX) {
        spdlog::error("JumpBehavior: minX ({}) 应小于 maxX ({})。行为可能不正确。", minX, maxX);
        m_patrolMinX = m_patrolMaxX; // 修正为相等，避免逻辑错误
    }

    // 确保垂直跳跃速度是负数（向上）
    if (m_jumpVelocity.y > 0.0f) {
        spdlog::error("JumpBehavior: 垂直跳跃速度（{}）应为负数（向上）。已取相反数。",
                      m_jumpVelocity.y);
        m_jumpVelocity.y = -m_jumpVelocity.y;
    }

    // 确保跳跃间隔时间是正数
    if (m_jumpInterval <= 0.0f) {
        spdlog::error("JumpBehavior: 跳跃间隔时间（{}）应为正数。已修正为 2.0 秒。", m_jumpInterval);
        m_jumpInterval = 2.0f;
    }
}

