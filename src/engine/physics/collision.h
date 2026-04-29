#pragma once

namespace engine::component {
class ColliderComponent;
}

namespace engine::physics::collision {

/**
 * @brief 检查两个碰撞器组件是否重叠。
 * @param a 第一个碰撞器组件。
 * @param b 第二个碰撞器组件。
 * @return true 如果碰撞器组件重叠，否则为 false。
 */
bool checkCollision(const engine::component::ColliderComponent& a,
                    const engine::component::ColliderComponent& b);

/**
 * @brief 检查两个轴对齐包围盒 (AABB) 是否重叠。
 * @param aPos 第一个AABB的左上角坐标。
 * @param aSize 第一个AABB的尺寸。
 * @param bPos 第二个AABB的左上角坐标。
 * @param bSize 第二个AABB的尺寸。
 * @return true 如果AABB重叠，否则为 false。
 */
bool checkAabbOverlap(const glm::vec2& aPos,
                      const glm::vec2& aSize,
                      const glm::vec2& bPos,
                      const glm::vec2& bSize);

// 未来可以添加更多碰撞检测相关的函数

} // namespace engine::physics::collision
