#include "physics_engine.h"
#include "../component/collider_component.h"
#include "../component/physics_component.h"
#include "../component/transform_component.h"
#include "../object/game_object.h"
#include "collision.h"

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

void PhysicsEngine::registerCollisionLayer(engine::component::TileLayerComponent* layer)
{
    m_collisionTileLayers.push_back(layer);
    spdlog::trace("碰撞瓦片图层注册完成。");
}

void PhysicsEngine::unregisterCollisionLayer(engine::component::TileLayerComponent* layer)
{
    // 使用 remove-erase 范式安全地移除组件
    auto it = std::remove(m_collisionTileLayers.begin(), m_collisionTileLayers.end(), layer);
    m_collisionTileLayers.erase(it, m_collisionTileLayers.end());
    spdlog::trace("碰撞瓦片图层注销完成。");
}

void PhysicsEngine::update(float deltaTime)
{
    // 每帧开始时，清空上一帧的碰撞记录
    m_collisionPairs.clear();

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

        // 处理瓦片层碰撞（速度和位置的更新移入 resolveTileCollisions()）
        resolveTileCollisions(physicsComponent, deltaTime);
    }

    // 执行碰撞检测
    checkObjectCollisions();
}

void PhysicsEngine::checkObjectCollisions()
{
    for (size_t i{ 0 }; i < m_components.size(); ++i) {
        auto* physicsComponentA = m_components[i];
        if (!physicsComponentA || !physicsComponentA->isEnabled()) {
            continue;
        }

        auto* gameObjectA = physicsComponentA->owner();
        if (!gameObjectA) {
            continue;
        }

        auto* colliderComponentA = gameObjectA->getComponent<engine::component::ColliderComponent>();
        if (!colliderComponentA || !colliderComponentA->isActive()) {
            continue;
        }

        for (size_t j{ i + 1 }; j < m_components.size(); ++j) {
            auto* physicsComponentB = m_components[j];
            if (!physicsComponentB || !physicsComponentB->isEnabled()) {
                continue;
            }

            auto* gameObjectB = physicsComponentB->owner();
            if (!gameObjectB) {
                continue;
            }

            auto* colliderComponentB = gameObjectB
                                           ->getComponent<engine::component::ColliderComponent>();
            if (!colliderComponentB || !colliderComponentB->isActive()) {
                continue;
            }

            // 通过保护性测试后，正式执行逻辑
            if (collision::checkCollision(*colliderComponentA, *colliderComponentB)) {
                // 记录碰撞对
                m_collisionPairs.emplace_back(gameObjectA, gameObjectB);
            }
        }
    }
}

void PhysicsEngine::resolveTileCollisions(engine::component::PhysicsComponent* component,
                                          float deltaTime)
{
    // -- 检查组件是否有效 --
    auto* gameObject = component->owner();
    if (!gameObject) {
        return;
    }

    auto* transformComponent = gameObject->getComponent<engine::component::TransformComponent>();
    auto* colliderComponent = gameObject->getComponent<engine::component::ColliderComponent>();
    if (!transformComponent || !colliderComponent || !colliderComponent->isActive()
        || colliderComponent->isTrigger()) {
        return;
    }
}

} // namespace engine::physics
