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

void ColliderComponent::updateOffset()
{
    if (!m_collider) {
        return;
    }

    // 获取碰撞器的最小包围盒(AABB)尺寸
    auto colliderAabbSize{ m_collider->aabbSize() };
    // 如果 AABB 尺寸无效，偏移为 0
    if (colliderAabbSize.x <= 0.0f || colliderAabbSize.y <= 0.0f) {
        m_offset = glm::vec2{ 0.0f, 0.0f };
        return;
    }

    auto scaleFactor = m_transformComponent->scale();
    // 计算碰撞器左上角相对于 TransformComponent::m_position 的偏移量
    switch (m_alignment) {
    case engine::utils::Alignment::TopLeft:
        m_offset = glm::vec2{ 0.0f, 0.0f } * scaleFactor;
        break;
    case engine::utils::Alignment::TopCenter:
        m_offset = glm::vec2{ -colliderAabbSize.x / 2.0f, 0.0f } * scaleFactor;
        break;
    case engine::utils::Alignment::TopRight:
        m_offset = glm::vec2{ -colliderAabbSize.x, 0.0f } * scaleFactor;
        break;
    case engine::utils::Alignment::CenterLeft:
        m_offset = glm::vec2{ 0.0f, -colliderAabbSize.y / 2.0f } * scaleFactor;
        break;
    case engine::utils::Alignment::Center:
        m_offset = glm::vec2{ -colliderAabbSize.x / 2.0f, -colliderAabbSize.y / 2.0f }
                   * scaleFactor;
        break;
    case engine::utils::Alignment::CenterRight:
        m_offset = glm::vec2{ -colliderAabbSize.x, -colliderAabbSize.y / 2.0f } * scaleFactor;
        break;
    case engine::utils::Alignment::BottomLeft:
        m_offset = glm::vec2{ 0.0f, -colliderAabbSize.y } * scaleFactor;
        break;
    case engine::utils::Alignment::BottomCenter:
        m_offset = glm::vec2{ -colliderAabbSize.x / 2.0f, -colliderAabbSize.y } * scaleFactor;
        break;
    case engine::utils::Alignment::BottomRight:
        m_offset = glm::vec2{ -colliderAabbSize.x, -colliderAabbSize.y } * scaleFactor;
        break;
    case engine::utils::Alignment::None:
    default:
        break;
    }
}

engine::utils::Rect ColliderComponent::worldAabb() const
{
    if (!m_transformComponent || !m_collider) {
        return engine::utils::Rect{ glm::vec2{ 0.0f, 0.0f }, glm::vec2{ 0.0f, 0.0f } };
    }

    // 计算最小包围盒的左上角坐标
    const glm::vec2 topLeftPos{ m_transformComponent->position() + m_offset };
    // 计算最小包围盒的尺寸
    const glm::vec2 aabSize{ m_collider->aabbSize() };
    const glm::vec2 scaleFactor{ m_transformComponent->scale() };
    glm::vec2 scaledSize{ aabSize * scaleFactor };
    //返回最小包围盒的 Rect
    return engine::utils::Rect{ topLeftPos, scaledSize };
}

void ColliderComponent::setAlignment(engine::utils::Alignment anchor)
{
    m_alignment = anchor;
    if (m_transformComponent && m_collider) {
        updateOffset();
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
    updateOffset();
}

} // namespace engine::component
