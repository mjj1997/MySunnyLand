#include "transform_component.h"
#include "../object/game_object.h"
#include "sprite_component.h"

namespace engine::component {

void TransformComponent::setScale(const glm::vec2& scale)
{
    m_scale = scale;
    // 应用缩放时应同步更新Sprite偏移量
    if (m_owner) {
        auto* spriteComponent = m_owner->getComponent<SpriteComponent>();
        if (spriteComponent) {
            spriteComponent->updateOffset();
        }
    }
}

} // namespace engine::component
