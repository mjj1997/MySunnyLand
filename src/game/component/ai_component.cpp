#include "ai_component.h"
#include "ai/ai_behavior_base.h"

#include "../../engine/component/animation_component.h"
#include "../../engine/component/audio_component.h"
#include "../../engine/component/health_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/object/game_object.h"

#include <spdlog/spdlog.h>

namespace game::component {

AiComponent::~AiComponent() = default;

bool AiComponent::takeDamage(int damageAmount)
{
    bool success{ false };
    if (auto* healthComponent = m_owner->getComponent<engine::component::HealthComponent>();
        healthComponent) {
        success = healthComponent->takeDamage(damageAmount);
        // TODO: 可以设置受伤、死亡后的行为
    }

    return success;
}

bool AiComponent::isAlive() const
{
    if (auto* healthComponent = m_owner->getComponent<engine::component::HealthComponent>();
        healthComponent) {
        return healthComponent->isAlive();
    }

    return true; // 如果没有生命组件，默认认为是活着的
}

void AiComponent::setBehavior(std::unique_ptr<ai::AiBehaviorBase> behavior)
{
    m_currentBehavior = std::move(behavior);
    spdlog::debug("GameObject '{}' 上的 AiComponent 设置了新的行为。",
                  m_owner != nullptr ? m_owner->name() : "未知");
    if (m_currentBehavior) {
        m_currentBehavior->enter(*this); // 调用新行为的 enter 方法
    }
}

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
    m_audioComponent = m_owner->getComponent<engine::component::AudioComponent>();

    // 检查所有必需组件是否都存在(音效组件并非必须存在)
    if (m_transformComponent == nullptr || m_physicsComponent == nullptr
        || m_spriteComponent == nullptr || m_animationComponent == nullptr) {
        spdlog::error("GameObject {} 上的 AiComponent 缺少必需组件！", m_owner->name());
    }
}

void AiComponent::update(float deltaTime, engine::core::Context& context)
{
    // 将更新委托给当前的行为策略
    if (m_currentBehavior) {
        m_currentBehavior->update(deltaTime, *this);
    } else {
        spdlog::warn("GameObject '{}' 上的 AiComponent 没有设置行为。",
                     m_owner != nullptr ? m_owner->name() : "未知");
    }
}

} // namespace game::component
