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

} // namespace engine::component
