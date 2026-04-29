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

// 未来可以添加更多碰撞检测相关的函数

} // namespace engine::physics::collision
