#include "collision.h"
#include "../component/collider_component.h"
#include "../component/transform_component.h"

namespace engine::physics {

bool collision::checkCollision(const engine::component::ColliderComponent& a,
                               const engine::component::ColliderComponent& b)
{
    // 获取碰撞器及对应 TransformComponent 信息
    auto aCollider = a.collider();
    auto bCollider = b.collider();
    auto aTransformComponent = a.transformComponent();
    auto bTransformComponent = b.transformComponent();

    // 先判断最小包围盒是否碰撞，如果没有碰撞，那一定是返回false (不考虑AABB的旋转)
    auto aSize = aCollider->aabbSize() * aTransformComponent->scale();
    auto bSize = bCollider->aabbSize() * bTransformComponent->scale();
    auto aPos = aTransformComponent->position() + a.offset();
    auto bPos = bTransformComponent->position() + b.offset();
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
    // AABB vs Circle: 判断圆心到AABB的最邻近点是否在圆内
    // Circle vs AABB

    return false;
}

bool collision::checkAabbOverlap(const glm::vec2& aPos,
                                 const glm::vec2& aSize,
                                 const glm::vec2& bPos,
                                 const glm::vec2& bSize)
{
    // 检查两个AABB是否重叠，根据AABB的左上角坐标和尺寸判断
    if (aPos.x + aSize.x <= bPos.x || aPos.x >= bPos.x + bSize.x || aPos.y + aSize.y <= bPos.y
        || aPos.y >= bPos.y + bSize.y) {
        return false;
    }
    return true;
}

bool collision::checkCircleOverlap(const glm::vec2& aCenter,
                                   const float aRadius,
                                   const glm::vec2& bCenter,
                                   const float bRadius)
{
    return (glm::length(aCenter - bCenter) < aRadius + bRadius);
}

} // namespace engine::physics
