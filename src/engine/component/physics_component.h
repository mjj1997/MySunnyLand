#pragma once

#include "component_base.h"

#include <glm/vec2.hpp>

namespace engine::physics {
class PhysicsEngine;
}

namespace engine::component {
class TransformComponent;

/**
 * @brief 管理 GameObject 的物理属性
 *
 * 存储速度、质量、力和重力设置。
 * 与 PhysicsEngine 交互。
 */
class PhysicsComponent final : public ComponentBase
{
    friend class engine::object::GameObject;

public:
    /**
     * @brief 构造函数  
     * 
     * @param physicsEngine 指向 PhysicsEngine 的指针，不能为 nullptr
     * @param mass 物体质量，默认1.0
     * @param isGravityEnabled 物体是否受重力影响，默认 true
     */
    explicit PhysicsComponent(engine::physics::PhysicsEngine* physicsEngine,
                              float mass = 1.0f,
                              bool isGravityEnabled = true);
    ~PhysicsComponent() override = default;

    // 删除复制/移动操作
    PhysicsComponent(const PhysicsComponent&) = delete;
    PhysicsComponent& operator=(const PhysicsComponent&) = delete;
    PhysicsComponent(PhysicsComponent&&) = delete;
    PhysicsComponent& operator=(PhysicsComponent&&) = delete;

    // PhysicsEngine使用的物理方法
    ///< @brief 添加力
    void addForce(const glm::vec2& force)
    {
        if (m_enabled)
            m_force += force;
    }
    void clearForce() { m_force = { 0.0f, 0.0f }; } ///< @brief 清除力

    // 设置器/获取器
    ///< @brief 获取TransformComponent指针
    TransformComponent* transformComponent() const { return m_transformComponent; }
    const glm::vec2& velocity() const { return m_velocity; }     ///< @brief 获取当前速度
    const glm::vec2& force() const { return m_force; }           ///< @brief 获取当前力
    float mass() const { return m_mass; }                        ///< @brief 获取质量
    bool isGravityEnabled() const { return m_isGravityEnabled; } ///< @brief 获取组件是否受重力影响
    bool isEnabled() const { return m_enabled; }                 ///< @brief 获取组件是否启用

    void setVelocity(const glm::vec2& velocity) { m_velocity = velocity; } ///< @brief 设置速度
    ///< @brief 设置质量，质量不能为负
    void setMass(float mass) { m_mass = (mass >= 0.0f) ? mass : 1.0f; }
    ///< @brief 设置组件是否受重力影响
    void setGravityEnabled(bool isGravityEnabled) { m_isGravityEnabled = isGravityEnabled; }
    void setEnabled(bool enabled) { m_enabled = enabled; } ///< @brief 设置组件是否启用

protected:
    // 核心循环方法
    void init() override;
    void update(double deltaTime, engine::core::Context&) override {}
    void clean() override;

private:
    engine::physics::PhysicsEngine* m_physicsEngine{ nullptr }; ///< @brief 指向 PhysicsEngine 的指针
    TransformComponent* m_transformComponent{ nullptr }; ///< @brief TransformComponent 的缓存指针

    glm::vec2 m_velocity{ 0.0f, 0.0f }; ///< @brief 物体的速度
    glm::vec2 m_force{ 0.0f, 0.0f };    ///< @brief 当前帧受到的力
    float m_mass{ 1.0f };               ///< @brief 物体质量（默认1.0）
    bool m_isGravityEnabled{ true };    ///< @brief 物体是否受重力影响
    bool m_enabled{ true };             ///< @brief 组件是否激活
};

} // namespace engine::component
