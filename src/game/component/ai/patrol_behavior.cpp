#include "patrol_behavior.h"
#include "../ai_component.h"

#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include "../../../engine/component/transform_component.h"

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

void PatrolBehavior::update(float deltaTime, AiComponent& aiComponent)
{
    // 获取必要组件
    auto* physicsComponent = aiComponent.physicsComponent();
    auto* transformComponent = aiComponent.transformComponent();
    auto* spriteComponent = aiComponent.spriteComponent();
    if (!physicsComponent || !transformComponent || !spriteComponent) {
        spdlog::error("PatrolBehavior: 缺少必要组件，无法执行巡逻行为。");
        return;
    }

    // 检查是否碰撞和是否到达巡逻范围
    auto currentX = transformComponent->position().x;
    if (physicsComponent->isCollidedRight() || currentX >= m_patrolMaxX) {
        // 如果撞右墙或到达巡逻范围右边界，就转向左
        glm::vec2 newVelocity{ -m_moveSpeed, physicsComponent->velocity().y };
        physicsComponent->setVelocity(newVelocity);
    } else if (physicsComponent->isCollidedLeft() || currentX <= m_patrolMinX) {
        // 如果撞左墙或到达巡逻范围左边界，就转向右
        glm::vec2 newVelocity{ m_moveSpeed, physicsComponent->velocity().y };
        physicsComponent->setVelocity(newVelocity);
    }
}

} // namespace game::component::ai
