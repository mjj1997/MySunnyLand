#include "patrol_behavior.h"

#include <spdlog/spdlog.h>

namespace game::component::ai {

PatrolBehavior::PatrolBehavior(float minX, float maxX, float speed)
    : m_patrolMinX{ minX }
    , m_patrolMaxX{ maxX }
    , m_moveSpeed{ speed }
{
    if (m_patrolMinX > m_patrolMaxX) {
        spdlog::error("PatrolBehavior: minX ({}) 应小于 maxX ({})。行为可能不正确。", minX, maxX);
        m_patrolMinX = m_patrolMaxX; // 修正为相等，避免逻辑错误
    }
}

} // namespace game::component::ai
