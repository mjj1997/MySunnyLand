#include "physics_engine.h"
#include "../component/physics_component.h"
#include "../component/transform_component.h"

#include <glm/common.hpp>
#include <spdlog/spdlog.h>

namespace engine::physics {

void PhysicsEngine::registerComponent(engine::component::PhysicsComponent* component)
{
    m_components.push_back(component);
    spdlog::trace("物理组件注册完成。");
}

void PhysicsEngine::unregisterComponent(engine::component::PhysicsComponent* component)
{
    // 使用 remove-erase 范式安全地移除组件
    auto it = std::remove(m_components.begin(), m_components.end(), component);
    m_components.erase(it, m_components.end());
    spdlog::trace("物理组件注销完成。");
}

void PhysicsEngine::update(float deltaTime)
{
    // 遍历所有注册的物理组件
    for (auto* physicsComponent : m_components) {
        // 检查组件是否有效和启用
        if (!physicsComponent || !physicsComponent->isEnabled()) {
            continue;
        }

        // 应用重力 (如果组件受重力影响)：F = g * m
        if (physicsComponent->isGravityEnabled()) {
            physicsComponent->addForce(m_gravity * physicsComponent->mass());
        }
        /* 还可以添加其它力影响，比如风力、摩擦力等，目前不考虑 */

        // 更新速度： v += a * dt，其中 a = F / m
        auto newVelocity = physicsComponent->velocity()
                           + (physicsComponent->force() / physicsComponent->mass()) * deltaTime;
        physicsComponent->setVelocity(newVelocity);
        physicsComponent->clearForce(); // 清除当前帧的力

        // 更新位置：S += v * dt
        auto* transforomComponent = physicsComponent->transformComponent();
        if (transforomComponent) {
            transforomComponent->translate(physicsComponent->velocity() * deltaTime);
        }

        // 限制最大速度：v = min(v, max_speed)
        physicsComponent->setVelocity(
            glm::clamp(physicsComponent->velocity(), -m_maxSpeed, m_maxSpeed));
    }
}

} // namespace engine::physics
