#include "updown_behavior.h"
#include "../ai_component.h"

#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/transform_component.h"

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

void UpDownBehavior::update(float deltaTime, AiComponent& aiComponent)
{
    // 获取必要组件
    auto* physicsComponent = aiComponent.physicsComponent();
    auto* transformComponent = aiComponent.transformComponent();
    if (!physicsComponent || !transformComponent) {
        spdlog::error("UpDownBehavior: 缺少必要组件，无法执行上下移动行为。");
        return;
    }

    // 检查是否碰撞和是否到达巡逻范围
    auto currentY = transformComponent->position().y;
    if (physicsComponent->isCollidedAbove() || currentY >= m_patrolMaxY) {
        // 如果撞上障碍物或到达巡逻范围上边界，就转向下
        glm::vec2 newVelocity{ physicsComponent->velocity().x, m_moveSpeed };
        physicsComponent->setVelocity(newVelocity);
        m_movingDown = true;
    } else if (physicsComponent->isCollidedBelow() || currentY <= m_patrolMinY) {
        // 如果撞下障碍物或到达巡逻范围下边界，就转向上
        glm::vec2 newVelocity{ physicsComponent->velocity().x, -m_moveSpeed };
        physicsComponent->setVelocity(newVelocity);
        m_movingDown = false;
    }

    /* 不需要翻转精灵图 */
}

} // namespace game::component::ai
