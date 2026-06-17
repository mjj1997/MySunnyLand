#include "transform_component.h"
#include "../object/game_object.h"
#include "collider_component.h"
#include "sprite_component.h"

namespace engine::component {

void TransformComponent::setScale(glm::vec2 scale)
{
    m_scale = std::move(scale);
    if (m_owner) {
        // 应用缩放时应同步更新Sprite偏移量
        auto* spriteComponent = m_owner->getComponent<SpriteComponent>();
        if (spriteComponent) {
            spriteComponent->updateOffset();
        }
        // 应用缩放时应同步更新Collider偏移量
        auto* colliderComponent = m_owner->getComponent<ColliderComponent>();
        if (colliderComponent) {
            colliderComponent->updateOffset();
        }
    }
}

} // namespace engine::component
