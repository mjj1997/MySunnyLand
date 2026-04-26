#include "physics_component.h"
#include "../object/game_object.h"
#include "../physics/physics_engine.h"
#include "transform_component.h"

#include <spdlog/spdlog.h>

namespace engine::component {

PhysicsComponent::PhysicsComponent(engine::physics::PhysicsEngine* physicsEngine,
                                   float mass,
                                   bool isGravityEnabled)
    : m_physicsEngine{ physicsEngine }
    , m_mass{ mass >= 0.0f ? mass : 1.0f }
    , m_isGravityEnabled{ isGravityEnabled }
{
    if (!physicsEngine) {
        spdlog::error("PhysicsComponent构造函数中，PhysicsEngine指针不能为nullptr！");
    }
    spdlog::trace("物理组件创建完成，质量: {}, 使用重力: {}", m_mass, m_isGravityEnabled);
}

void PhysicsComponent::init()
{
    if (!m_owner) {
        spdlog::error("物理组件初始化前，需要一个 GameObject 作为所有者！");
        return;
    }
    if (!m_physicsEngine) {
        spdlog::error("物理组件初始化时，PhysicsEngine 未正确初始化。");
        return;
    }

    m_transformComponent = m_owner->getComponent<TransformComponent>();
    if (!m_transformComponent) {
        spdlog::warn("物理组件初始化时，同一 GameObject 上没有找到 TransformComponent。");
    }

    // 注册到 PhysicsEngine
    m_physicsEngine->registerComponent(this);
    spdlog::trace("物理组件初始化完成。");
}

void PhysicsComponent::clean()
{
    // 从 PhysicsEngine 注销
    m_physicsEngine->unregisterComponent(this);
    spdlog::trace("物理组件清理完成。");
}

} // namespace engine::component
