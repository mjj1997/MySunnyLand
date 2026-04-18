#include "sprite_component.h"
#include "../object/game_object.h"
#include "../resource/resource_manager.h"
#include "transform_component.h"

#include <spdlog/spdlog.h>

namespace engine::component {

SpriteComponent::SpriteComponent(const std::string& textureId,
                                 engine::resource::ResourceManager& resourceManager,
                                 engine::utils::Alignment alignment,
                                 std::optional<SDL_FRect> sourceRect,
                                 bool isFlipped)
    : m_resourceManager{ &resourceManager }
    , m_sprite{ textureId, sourceRect, isFlipped }
    , m_alignment{ alignment }
{
    if (!m_resourceManager) {
        spdlog::critical("创建 SpriteComponent 时 ResourceManager 为空！，此组件将无效。");
        // 不要在游戏主循环中使用 try...catch / throw，会极大影响性能
    }
    // m_offset 和 m_spriteSize 将在 init 中计算
    spdlog::trace("创建 SpriteComponent，纹理ID: {}", textureId);
}

void SpriteComponent::init()
{
    if (!m_owner) {
        spdlog::error("SpriteComponent 在初始化前未设置所有者 GameObject。");
        return;
    }

    m_transformComponent = m_owner->getComponent<TransformComponent>();
    if (!m_transformComponent) {
        spdlog::warn("GameObject '{}' 上的 SpriteComponent 需要一个 TransformComponent，但未找到。",
                     m_owner->name());
        // Sprite 没有 Transform，无法计算偏移和渲染，直接返回
        return;
    }

    // TODO: 更新大小及偏移
}

void SpriteComponent::updateSpriteSize()
{
    if (!m_resourceManager) {
        spdlog::error("ResourceManager 为空！无法获取纹理尺寸。");
        return;
    }

    if (m_sprite.sourceRect().has_value()) {
        const auto& srcRect = m_sprite.sourceRect().value();
        m_spriteSize = glm::vec2{ srcRect.w, srcRect.h };
    } else {
        m_spriteSize = m_resourceManager->getTextureSize(m_sprite.textureId());
    }
}

void SpriteComponent::updateOffset()
{
    // 如果精灵尺寸无效，偏移为 0
    if (m_spriteSize.x <= 0 || m_spriteSize.y <= 0) {
        m_offset = glm::vec2{ 0.0f, 0.0f };
        return;
    }

    auto scaleFactor = m_transformComponent->scale();
    // 计算精灵左上角相对于 TransformComponent::m_position 的偏移量
    switch (m_alignment) {
    case engine::utils::Alignment::TopLeft:
        m_offset = glm::vec2{ 0.0f, 0.0f } * scaleFactor;
        break;
    case engine::utils::Alignment::TopCenter:
        m_offset = glm::vec2{ -m_spriteSize.x / 2.0f, 0.0f } * scaleFactor;
        break;
    case engine::utils::Alignment::TopRight:
        m_offset = glm::vec2{ -m_spriteSize.x, 0.0f } * scaleFactor;
        break;
    case engine::utils::Alignment::CenterLeft:
        m_offset = glm::vec2{ 0.0f, -m_spriteSize.y / 2.0f } * scaleFactor;
        break;
    case engine::utils::Alignment::Center:
        m_offset = glm::vec2{ -m_spriteSize.x / 2.0f, -m_spriteSize.y / 2.0f } * scaleFactor;
        break;
    case engine::utils::Alignment::CenterRight:
        m_offset = glm::vec2{ -m_spriteSize.x, -m_spriteSize.y / 2.0f } * scaleFactor;
        break;
    case engine::utils::Alignment::BottomLeft:
        m_offset = glm::vec2{ 0.0f, -m_spriteSize.y } * scaleFactor;
        break;
    case engine::utils::Alignment::BottomCenter:
        m_offset = glm::vec2{ -m_spriteSize.x / 2.0f, -m_spriteSize.y } * scaleFactor;
        break;
    case engine::utils::Alignment::BottomRight:
        m_offset = glm::vec2{ -m_spriteSize.x, -m_spriteSize.y } * scaleFactor;
        break;
    case engine::utils::Alignment::None:
    default:
        break;
    }
}

} // namespace engine::component
