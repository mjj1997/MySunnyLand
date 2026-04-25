#include "physics_engine.h"

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

} // namespace engine::physics
