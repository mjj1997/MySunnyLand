#include "collision.h"
#include "../component/collider_component.h"
#include "../component/transform_component.h"

namespace engine::physics {

bool collision::checkCollision(const engine::component::ColliderComponent& aColliderComponent,
                               const engine::component::ColliderComponent& bColliderComponent)
{
    // 获取碰撞器及对应 TransformComponent 信息
    const auto* aCollider = aColliderComponent.collider();
    const auto* bCollider = bColliderComponent.collider();
    const auto* aTransformComponent = aColliderComponent.transformComponent();
    const auto* bTransformComponent = bColliderComponent.transformComponent();

    // 先判断最小包围盒是否碰撞，如果没有碰撞，那一定是返回false (不考虑AABB的旋转)
    auto aSize = aCollider->aabbSize() * aTransformComponent->scale();
    auto bSize = bCollider->aabbSize() * bTransformComponent->scale();
    auto aPos = aTransformComponent->position() + aColliderComponent.offset();
    auto bPos = bTransformComponent->position() + bColliderComponent.offset();
    if (!checkAabbOverlap(aPos, aSize, bPos, bSize)) {
        return false;
    }

    // --- 如果最小包围盒有碰撞，再进行更细致的判断 ---
    // AABB vs AABB, 直接返回真
    if (aCollider->type() == engine::physics::ColliderType::Aabb
        && bCollider->type() == engine::physics::ColliderType::Aabb) {
        return true;
    }
    // Circle vs Circle: 判断两个圆心距离是否小于两个圆的半径之和
    else if (aCollider->type() == engine::physics::ColliderType::Circle
             && bCollider->type() == engine::physics::ColliderType::Circle) {
        auto aCenter = aPos + 0.5F * aSize; // 圆心位置
        auto bCenter = bPos + 0.5F * bSize;
        auto aRadius = 0.5F * aSize.x; // 圆的半径等于AABB的一半宽度
        auto bRadius = 0.5F * bSize.x;
        return checkCircleOverlap(aCenter, aRadius, bCenter, bRadius);
    }
    // AABB vs Circle: 判断圆心到AABB的最邻近点是否在圆内
    else if (aCollider->type() == engine::physics::ColliderType::Aabb
             && bCollider->type() == engine::physics::ColliderType::Circle) {
        auto bCenter = bPos + 0.5F * bSize;
        auto bRadius = 0.5F * bSize.x;
        auto nearestPoint = glm::clamp(bCenter, aPos, aPos + aSize); // 计算圆心到AABB的最邻近点
        return checkPointInCircle(nearestPoint, bCenter, bRadius);
    }
    // Circle vs AABB
    else if (aCollider->type() == engine::physics::ColliderType::Circle
             && bCollider->type() == engine::physics::ColliderType::Aabb) {
        auto aCenter = aPos + 0.5F * aSize;
        auto aRadius = 0.5F * aSize.x;
        auto nearestPoint = glm::clamp(aCenter, bPos, bPos + bSize); // 计算圆心到AABB的最邻近点
        return checkPointInCircle(nearestPoint, aCenter, aRadius);
    }

    return false;
}

bool collision::checkAabbOverlap(const glm::vec2& aPos,
                                 const glm::vec2& aSize,
                                 const glm::vec2& bPos,
                                 const glm::vec2& bSize)
{
    // 检查两个AABB是否重叠，根据AABB的左上角坐标和尺寸判断
    return (aPos.x + aSize.x > bPos.x) && (aPos.x < bPos.x + bSize.x) && (aPos.y + aSize.y > bPos.y)
           && (aPos.y < bPos.y + bSize.y);
}

bool collision::checkRectOverlap(const engine::utils::Rect& aRect, const engine::utils::Rect& bRect)
{
    return checkAabbOverlap(aRect.position, aRect.size, bRect.position, bRect.size);
}

bool collision::checkCircleOverlap(const glm::vec2& aCenter,
                                   const float aRadius,
                                   const glm::vec2& bCenter,
                                   const float bRadius)
{
    return (glm::length(aCenter - bCenter) < aRadius + bRadius);
}

bool collision::checkPointInCircle(const glm::vec2& point,
                                   const glm::vec2& center,
                                   const float radius)
{
    return (glm::length(point - center) < radius);
}

} // namespace engine::physics
