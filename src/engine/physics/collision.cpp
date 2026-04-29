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

    // 先计算最小包围盒是否碰撞，如果没有碰撞，那一定是返回false (不考虑AABB的旋转)

    // --- 如果最小包围盒有碰撞，再进行更细致的判断 ---
    // AABB vs AABB, 直接返回真
    // Circle vs Circle: 判断两个圆心距离是否小于两个圆的半径之和
    // AABB vs Circle: 判断圆心到AABB的最邻近点是否在圆内
    // Circle vs AABB

    return false;
}

} // namespace engine::physics
