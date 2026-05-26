#include "ai_component.h"

#include "../../engine/component/animation_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/object/game_object.h"

#include <spdlog/spdlog.h>

namespace game::component {

void AiComponent::init()
{
    if (m_owner == nullptr) {
        spdlog::error("AiComponent 没有所属的游戏对象！");
        return;
    }

    // 获取并缓存游戏对象的组件指针
    m_transformComponent = m_owner->getComponent<engine::component::TransformComponent>();
    m_physicsComponent = m_owner->getComponent<engine::component::PhysicsComponent>();
    m_spriteComponent = m_owner->getComponent<engine::component::SpriteComponent>();
    m_animationComponent = m_owner->getComponent<engine::component::AnimationComponent>();

    // 检查所有必需组件是否都存在
    if (m_transformComponent == nullptr || m_physicsComponent == nullptr
        || m_spriteComponent == nullptr || m_animationComponent == nullptr) {
        spdlog::error("GameObject {} 上的 AiComponent 缺少必需组件！", m_owner->name());
    }
}

} // namespace game::component
