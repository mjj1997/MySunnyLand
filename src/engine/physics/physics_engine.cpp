#include "physics_engine.h"
#include "../component/collider_component.h"
#include "../component/physics_component.h"
#include "../component/tilelayer_component.h"
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
    // 设置物理引擎实例指针, 方便反注册
    layer->setPhysicsEngine(this);

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
                // 如果是可移动物体与 Solid 物体碰撞，则直接处理位置变化，不用记录碰撞对
                if (gameObjectA->tag() != "solid" && gameObjectB->tag() == "solid") {
                    resolveSolidCollisions(gameObjectA, gameObjectB);
                } else if (gameObjectA->tag() == "solid" && gameObjectB->tag() != "solid") {
                    resolveSolidCollisions(gameObjectB, gameObjectA);
                } else {
                    // 记录碰撞对
                    m_collisionPairs.emplace_back(gameObjectA, gameObjectB);
                }
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

    // 使用最小包围盒进行碰撞检测（简化）
    auto worldAabb = colliderComponent->worldAabb();
    auto objectPosition = worldAabb.position;
    auto objectSize = worldAabb.size;
    if (objectSize.x <= 0.0f || objectSize.y <= 0.0f) {
        return;
    }
    // -- 检查结束，正式开始处理 --

    auto displacement = component->velocity() * deltaTime;  // 计算物体在 deltaTime 内的位移
    auto newObjectPosition = objectPosition + displacement; // 计算物体在 deltaTime 后的新位置
    auto epsilon = 1.0f; // 检查右边缘和下边缘时，需要减1像素，否则会检查到下一行/列的瓦片

    for (auto* tileLayer : m_collisionTileLayers) {
        auto tileSize = tileLayer->tileSize();
        // 轴分离碰撞检测：先检查X方向是否有碰撞 (y方向使用初始值 objectPosition.y)
        if (displacement.x > 0.0f) { // 检查右侧碰撞，需要分别测试右上和右下角
            // 获取瓦片坐标的 X 方向分量。两块瓦片的 X 坐标相同
            // 右上角、右下角瓦片的 X 坐标
            auto tileXRight = static_cast<int>(
                std::floor((newObjectPosition.x + objectSize.x) / tileSize.x));
            // 获取瓦片坐标的 Y 方向分量。分为上和下两个部分
            // 右上角瓦片的 Y 坐标
            auto tileYTop = static_cast<int>(std::floor(objectPosition.y / tileSize.y));
            // 右上角瓦片类型
            auto tileTypeTopRight = tileLayer->tileTypeAt(glm::ivec2{ tileXRight, tileYTop });
            // 右下角瓦片的 Y 坐标
            auto tileYBottom = static_cast<int>(
                std::floor((objectPosition.y + objectSize.y - epsilon) / tileSize.y));
            // 右下角瓦片类型
            auto tileTypeBottomRight = tileLayer->tileTypeAt(glm::ivec2{ tileXRight, tileYBottom });

            if (tileTypeTopRight == engine::component::TileType::Solid
                || tileTypeBottomRight == engine::component::TileType::Solid) {
                // 撞墙了！速度归零，x方向移动到贴着墙的位置
                newObjectPosition.x = tileXRight * tileSize.x - objectSize.x;
                component->setVelocity({ 0.0f, component->velocity().y });
            }
        } else if (displacement.x < 0.0f) { // 检查左侧碰撞，需要分别测试左上和左下角
            // 获取瓦片坐标的 X 方向分量。两块瓦片的 X 坐标相同
            // 左上角、左下角瓦片的 X 坐标
            auto tileXLeft = static_cast<int>(std::floor(newObjectPosition.x / tileSize.x));
            // 获取瓦片坐标的 Y 方向分量。分为上和下两个部分
            // 左上角瓦片的 Y 坐标
            auto tileYTop = static_cast<int>(std::floor(objectPosition.y / tileSize.y));
            // 左上角瓦片类型
            auto tileTypeTopLeft = tileLayer->tileTypeAt(glm::ivec2{ tileXLeft, tileYTop });
            // 左下角瓦片的 Y 坐标
            auto tileYBottom = static_cast<int>(
                std::floor((objectPosition.y + objectSize.y - epsilon) / tileSize.y));
            // 左下角瓦片类型
            auto tileTypeBottomLeft = tileLayer->tileTypeAt(glm::ivec2{ tileXLeft, tileYBottom });

            if (tileTypeTopLeft == engine::component::TileType::Solid
                || tileTypeBottomLeft == engine::component::TileType::Solid) {
                // 撞墙了！速度归零，x方向移动到贴着墙的位置
                // 向左撞墙是贴在瓦片的右侧, 所以要 +1
                newObjectPosition.x = (tileXLeft + 1) * tileSize.x;
                component->setVelocity(glm::vec2{ 0.0f, component->velocity().y });
            }
        }
        // 轴分离碰撞检测：再检查Y方向是否有碰撞 (x方向使用初始值 objectPosition.x)
        if (displacement.y > 0.0f) {
            // 检查底部碰撞，需要分别测试左下和右下角
            // 获取瓦片坐标的 Y 方向分量。两块瓦片的 Y 坐标相同
            auto tileYBottom = static_cast<int>(
                std::floor((newObjectPosition.y + objectSize.y) / tileSize.y));
            // 获取瓦片坐标的 X 方向分量。分为左和右两个部分
            // 左下角瓦片的 X 坐标
            auto tileXLeft = static_cast<int>(std::floor(objectPosition.x / tileSize.x));
            // 左下角瓦片类型
            auto tileTypeBottomLeft = tileLayer->tileTypeAt(glm::ivec2{ tileXLeft, tileYBottom });
            // 右下角瓦片的 X 坐标
            auto tileXRight = static_cast<int>(
                std::floor((objectPosition.x + objectSize.x - epsilon) / tileSize.x));
            // 右下角瓦片类型
            auto tileTypeBottomRight = tileLayer->tileTypeAt(glm::ivec2{ tileXRight, tileYBottom });

            if (tileTypeBottomLeft == engine::component::TileType::Solid
                || tileTypeBottomRight == engine::component::TileType::Solid) {
                // 到达地面！速度归零，y方向移动到贴着地面的位置
                newObjectPosition.y = tileYBottom * tileSize.y - objectSize.y;
                component->setVelocity({ component->velocity().x, 0.0f });
            }
        } else if (displacement.y < 0.0f) {
            // 检查顶部碰撞，需要分别测试左上和右上角
            // 获取瓦片坐标的 Y 方向分量。两块瓦片的 Y 坐标相同
            auto tileYTop = static_cast<int>(std::floor(newObjectPosition.y / tileSize.y));
            // 获取瓦片坐标的 X 方向分量。分为左和右两个部分
            // 左上角瓦片的 X 坐标
            auto tileXLeft = static_cast<int>(std::floor(objectPosition.x / tileSize.x));
            // 左上角瓦片类型
            auto tileTypeTopLeft = tileLayer->tileTypeAt(glm::ivec2{ tileXLeft, tileYTop });
            // 右上角瓦片的 X 坐标
            auto tileXRight = static_cast<int>(
                std::floor((objectPosition.x + objectSize.x - epsilon) / tileSize.x));
            // 右上角瓦片类型
            auto tileTypeTopRight = tileLayer->tileTypeAt(glm::ivec2{ tileXRight, tileYTop });

            if (tileTypeTopLeft == engine::component::TileType::Solid
                || tileTypeTopRight == engine::component::TileType::Solid) {
                // 撞到天花板！速度归零，y方向移动到贴着天花板的位置
                // 向上撞墙是贴在瓦片的底部, 所以要 +1
                newObjectPosition.y = (tileYTop + 1) * tileSize.y;
                component->setVelocity({ component->velocity().x, 0.0f });
            }
        }
    }
    // 更新物体位置，并限制最大速度
    transformComponent->translate(newObjectPosition - objectPosition);
    component->setVelocity(glm::clamp(component->velocity(), -m_maxSpeed, m_maxSpeed));
}

void PhysicsEngine::resolveSolidCollisions(engine::object::GameObject* movingObject,
                                           engine::object::GameObject* solidObject)
{}

} // namespace engine::physics
