#include "collider_component.h"
#include "../object/game_object.h"
#include "transform_component.h"

#include <spdlog/spdlog.h>

namespace engine::component {

ColliderComponent::ColliderComponent(std::unique_ptr<engine::physics::ColliderBase> collider,
                                     engine::utils::Alignment alignment,
                                     bool isTrigger,
                                     bool isActive)
    : m_collider{ std::move(collider) }
    , m_alignment{ alignment }
    , m_isTrigger{ isTrigger }
    , m_isActive{ isActive }
{
    if (!m_collider) {
        spdlog::error("创建 ColliderComponent 时传入了空的碰撞器！");
    }
}

void ColliderComponent::setAlignment(engine::utils::Alignment anchor)
{
    m_alignment = anchor;
    if (m_transformComponent && m_collider) {
        // TODO: 更新偏移量
    }
}

void ColliderComponent::init()
{
    if (!m_owner) {
        spdlog::error("ColliderComponent 在初始化前未设置所有者 GameObject。");
        return;
    }

    m_transformComponent = m_owner->getComponent<TransformComponent>();
    if (!m_transformComponent) {
        spdlog::error(
            "GameObject '{}' 上的 ColliderComponent 需要一个 TransformComponent，但未找到。",
            m_owner->name());
        return;
    }

    // 在获取 TransformComponent 后，更新偏移量
}

} // namespace engine::component
