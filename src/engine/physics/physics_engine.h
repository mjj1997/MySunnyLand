#pragma once

#include <glm/vec2.hpp>

#include <vector>

namespace engine::component {
class PhysicsComponent;
class TileLayerComponent;
} // namespace engine::component

namespace engine::object {
class GameObject;
}

namespace engine::physics {

/**
 * @brief 负责管理和模拟物理行为及碰撞检测。
 */
class PhysicsEngine
{
public:
    PhysicsEngine() = default;

    // 禁止拷贝和移动
    PhysicsEngine(const PhysicsEngine&) = delete;
    PhysicsEngine& operator=(const PhysicsEngine&) = delete;
    PhysicsEngine(PhysicsEngine&&) = delete;
    PhysicsEngine& operator=(PhysicsEngine&&) = delete;

    void registerComponent(engine::component::PhysicsComponent* component); ///< @brief 注册物理组件
    void unregisterComponent(engine::component::PhysicsComponent* component); ///< @brief 注销物理组件

    // 如果瓦片层需要进行碰撞检测则注册。（不需要则不必注册）
    ///< @brief 注册用于碰撞检测的 TileLayerComponent
    void registerCollisionLayer(engine::component::TileLayerComponent* layer);
    ///< @brief 注销用于碰撞检测的 TileLayerComponent
    void unregisterCollisionLayer(engine::component::TileLayerComponent* layer);

    ///< @brief 核心循环：更新所有注册的物理组件的状态
    void update(float deltaTime);

    // 设置器/获取器
    ///< @brief 获取本帧发生的碰撞对 vector
    const auto& collisionPairs() const { return m_collisionPairs; }
    ///< @brief 设置全局重力加速度
    void setGravity(const glm::vec2& gravity) { m_gravity = gravity; }
    const glm::vec2& gravity() const { return m_gravity; } ///< @brief 获取当前的全局重力加速度
    void setMaxSpeed(float max_speed) { m_maxSpeed = max_speed; } ///< @brief 设置最大速度
    float maxSpeed() const { return m_maxSpeed; }                 ///< @brief 获取当前的最大速度

private:
    void checkObjectCollisions();

    ///< @brief 注册的物理组件容器，非拥有指针
    std::vector<engine::component::PhysicsComponent*> m_components;
    ///< @brief 注册的碰撞瓦片图层容器
    std::vector<engine::component::TileLayerComponent*> m_collisionTileLayers;

    ///< @brief 存储本帧发生的碰撞对
    std::vector<std::pair<engine::object::GameObject*, engine::object::GameObject*>> m_collisionPairs;

    ///< @brief 默认重力值 (像素/秒^2, 相当于 100像素 对应现实 1m)
    glm::vec2 m_gravity{ 0.0f, 980.0f };
    ///< @brief 最大速度 (像素/秒)
    float m_maxSpeed{ 500.0f };
};

} // namespace engine::physics
