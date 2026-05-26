#include "updown_behavior.h"

#include <spdlog/spdlog.h>

namespace game::component::ai {

UpDownBehavior::UpDownBehavior(float minY, float maxY, float speed)
    : m_patrolMinY{ minY }
    , m_patrolMaxY{ maxY }
    , m_moveSpeed{ speed }
{
    if (m_patrolMinY > m_patrolMaxY) {
        spdlog::error("UpDownBehavior: minY ({}) 应小于 maxY ({})。行为可能不正确。", minY, maxY);
        m_patrolMinY = m_patrolMaxY; // 修正为相等，避免逻辑错误
    }
}

} // namespace game::component::ai
