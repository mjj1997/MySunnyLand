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
            } else {
                // 处理右下角与斜坡的碰撞，根据瓦片中的斜坡高度调整物体位置
                auto widthRight = newObjectPosition.x + objectSize.x - tileXRight * tileSize.x;
                auto heightRight = getTileHeightAtWidth(widthRight, tileTypeBottomRight, tileSize);
                if (heightRight > 0.0f) {
                    // 如果有碰撞（右下角的世界 Y 坐标 > 斜坡高度的 Y 坐标），就让物体贴着斜坡表面
                    if (newObjectPosition.y + objectSize.y
                        > (tileYBottom + 1) * tileSize.y - heightRight) {
                        newObjectPosition.y = (tileYBottom + 1) * tileSize.y - heightRight
                                              - objectSize.y;
                    }
                }
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
            } else {
                // 处理左下角与斜坡的碰撞
                auto widthLeft = newObjectPosition.x - tileXLeft * tileSize.x;
                auto heightLeft = getTileHeightAtWidth(widthLeft, tileTypeBottomLeft, tileSize);
                if (heightLeft > 0.0f) {
                    // 如果有碰撞（左下角的世界 Y 坐标 > 斜坡高度的 Y 坐标），就让物体贴着斜坡表面
                    if (newObjectPosition.y + objectSize.y
                        > (tileYBottom + 1) * tileSize.y - heightLeft) {
                        newObjectPosition.y = (tileYBottom + 1) * tileSize.y - heightLeft
                                              - objectSize.y;
                    }
                }
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
                || tileTypeBottomRight == engine::component::TileType::Solid
                || tileTypeBottomLeft == engine::component::TileType::Unisolid
                || tileTypeBottomRight == engine::component::TileType::Unisolid) {
                // 到达地面！速度归零，y方向移动到贴着地面的位置
                newObjectPosition.y = tileYBottom * tileSize.y - objectSize.y;
                component->setVelocity({ component->velocity().x, 0.0f });
            } else {
                // 处理左下角、右下角与斜坡的碰撞
                auto widthLeft = objectPosition.x - tileXLeft * tileSize.x;
                auto heightLeft = getTileHeightAtWidth(widthLeft, tileTypeBottomLeft, tileSize);
                auto widthRight = objectPosition.x + objectSize.x - tileXRight * tileSize.x;
                auto heightRight = getTileHeightAtWidth(widthRight, tileTypeBottomRight, tileSize);
                auto height = glm::max(heightLeft, heightRight); // 取左右下角的最高高度进行检测
                if (height > 0.0f) {                             // 说明至少有一个角点处于斜坡瓦片上
                    // 如果有碰撞（角点的世界 Y 坐标 > 斜坡高度的 Y 坐标），就让物体贴着斜坡表面
                    if (newObjectPosition.y + objectSize.y
                        > (tileYBottom + 1) * tileSize.y - height) {
                        newObjectPosition.y = (tileYBottom + 1) * tileSize.y - height
                                              - objectSize.y;
                        // 到达地面！速度归零，y方向移动到贴着地面的位置
                        component->setVelocity({ component->velocity().x, 0.0f });
                    }
                }
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
{
    // 进入此函数前，已经检查了各个组件的有效性，因此直接进行计算
    auto* movingTransformComponent = movingObject
                                         ->getComponent<engine::component::TransformComponent>();
    auto* movingPhysicsComponent = movingObject->getComponent<engine::component::PhysicsComponent>();
    auto* movingColliderComponent = movingObject
                                        ->getComponent<engine::component::ColliderComponent>();
    auto* solidColliderComponent = solidObject->getComponent<engine::component::ColliderComponent>();

    // 这里只能获取期望位置，无法获取当前帧初始位置
    // 因为该方法调用时，已经在 resolveTileCollisions() 中重新计算了物体的位置，因此无法进行轴分离碰撞检测
    /* 未来可以进行重构，让这里可以获取初始位置。但是我们展示另外一种处理方法 */
    auto movingAabb = movingColliderComponent->worldAabb();
    auto solidAabb = solidColliderComponent->worldAabb();

    // --- 使用最小平移向量解决碰撞问题 ---
    auto movingCenter = movingAabb.position + movingAabb.size / 2.0f;
    auto solidCenter = solidAabb.position + solidAabb.size / 2.0f;
    // 计算两个包围盒的重叠部分
    auto overlap = glm::vec2{ movingAabb.size / 2.0f + solidAabb.size / 2.0f }
                   - glm::abs(movingCenter - solidCenter);
    // 如果重叠部分太小，则认为没有碰撞
    if (overlap.x < 0.1f && overlap.y < 0.1f) {
        return;
    }
    // 如果重叠部分在x方向上更小，则认为碰撞发生在x方向上（推出x方向平移向量最小）
    if (overlap.x < overlap.y) {
        // 移动物体在左边，让它贴着右边 Solid 物体（相当于向左移出重叠部分），y方向正常移动
        if (movingCenter.x < solidCenter.x) {
            // 设置位置
            movingTransformComponent->translate(glm::vec2{ -overlap.x, 0.0f });
            // 设置速度, 如果速度为正(向右移动)，则归零
            if (movingPhysicsComponent->velocity().x > 0.0f) {
                movingPhysicsComponent->setVelocity(
                    glm::vec2{ 0.0f, movingPhysicsComponent->velocity().y });
            }
        } else { // 移动物体在右边，让它贴着左边 Solid 物体（相当于向右移出重叠部分），y方向正常移动
            movingTransformComponent->translate(glm::vec2{ overlap.x, 0.0f });
            if (movingPhysicsComponent->velocity().x < 0.0f) {
                movingPhysicsComponent->setVelocity(
                    glm::vec2{ 0.0f, movingPhysicsComponent->velocity().y });
            }
        }
    } else { // 重叠部分在y方向上更小，则认为碰撞发生在y方向上（推出y方向平移向量最小）
        // 移动物体在上面，让它贴着下面 Solid 物体（相当于向上移出重叠部分），x方向正常移动
        if (movingCenter.y < solidCenter.y) {
            movingTransformComponent->translate(glm::vec2{ 0.0f, -overlap.y });
            if (movingPhysicsComponent->velocity().y > 0.0f) {
                movingPhysicsComponent->setVelocity(
                    glm::vec2{ movingPhysicsComponent->velocity().x, 0.0f });
            }
        } else { // 移动物体在下面，让它贴着上面 Solid 物体（相当于向下移出重叠部分），x方向正常移动
            movingTransformComponent->translate(glm::vec2{ 0.0f, overlap.y });
            if (movingPhysicsComponent->velocity().y < 0.0f) {
                movingPhysicsComponent->setVelocity(
                    glm::vec2{ 0.0f, movingPhysicsComponent->velocity().y });
            }
        }
    }
}

void PhysicsEngine::applyWorldBounds(engine::component::PhysicsComponent* physicsComponent)
{
    if (!physicsComponent || !m_worldBounds) {
        return;
    }

    // 只限定左、上、右边界，不限定下边界，以碰撞盒作为判断依据
    auto* gameObject = physicsComponent->owner();
    auto* colliderComponent = gameObject->getComponent<engine::component::ColliderComponent>();
    auto* transformComponent = gameObject->getComponent<engine::component::TransformComponent>();
    auto worldAabb = colliderComponent->worldAabb();
    auto objectPosition = worldAabb.position;
    auto objectSize = worldAabb.size;

    // 检查左边界
    if (objectPosition.x < m_worldBounds->position.x) {
        physicsComponent->setVelocity(glm::vec2{ 0.0f, physicsComponent->velocity().y });
        objectPosition.x = m_worldBounds->position.x;
    }
    // 检查上边界
    if (objectPosition.y < m_worldBounds->position.y) {
        physicsComponent->setVelocity(glm::vec2{ physicsComponent->velocity().x, 0.0f });
        objectPosition.y = m_worldBounds->position.y;
    }
    // 检查右边界
    if (objectPosition.x + objectSize.x > m_worldBounds->position.x + m_worldBounds->size.x) {
        physicsComponent->setVelocity(glm::vec2{ 0.0f, physicsComponent->velocity().y });
        objectPosition.x = m_worldBounds->position.x + m_worldBounds->size.x - objectSize.x;
    }

    // 更新物体位置(使用translate方法，新位置 - 旧位置)
    transformComponent->translate(objectPosition - worldAabb.position);
}

float PhysicsEngine::getTileHeightAtWidth(float width,
                                          engine::component::TileType type,
                                          glm::vec2 tileSize)
{
    auto ratio = glm::clamp(width / tileSize.x, 0.0f, 1.0f);
    switch (type) {
    case engine::component::TileType::Slope_0_1: // 左0   右1
        return ratio * tileSize.y;
    case engine::component::TileType::Slope_0_2: // 左0   右1/2
        return ratio * tileSize.y * 0.5f;
    case engine::component::TileType::Slope_2_1: // 左1/2 右1
        return ratio * tileSize.y * 0.5f + tileSize.y * 0.5f;
    case engine::component::TileType::Slope_1_0: // 左1   右0
        return (1.0f - ratio) * tileSize.y;
    case engine::component::TileType::Slope_2_0: // 左1/2 右0
        return (1.0f - ratio) * tileSize.y * 0.5f;
    case engine::component::TileType::Slope_1_2: // 左1   右1/2
        return (1.0f - ratio) * tileSize.y * 0.5f + tileSize.y * 0.5f;
    default:
        return 0.0f; // 默认返回0，表示没有斜坡
    }
}

} // namespace engine::physics
