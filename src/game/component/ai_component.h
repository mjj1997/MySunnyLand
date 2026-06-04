#pragma once

#include "../../engine/component/component_base.h"

#include <memory>

namespace game::component::ai {
class AiBehaviorBase;
}

namespace engine::component {
class TransformComponent;
class PhysicsComponent;
class SpriteComponent;
class AnimationComponent;
class AudioComponent;
} // namespace engine::component

namespace game::component {

/**
 * @brief 负责管理 GameObject 的 AI 行为。
 *
 * 使用策略模式，持有一个具体的 AiBehavior 实例来执行实际的 AI 逻辑。
 * 提供对 GameObject 其他关键组件的访问。
 */
class AiComponent final : public engine::component::ComponentBase
{
    friend class engine::object::GameObject;

public:
    AiComponent() = default;
    ~AiComponent() override = default;

    // 禁止拷贝和移动
    AiComponent(const AiComponent&) = delete;
    AiComponent& operator=(const AiComponent&) = delete;
    AiComponent(AiComponent&&) = delete;
    AiComponent& operator=(AiComponent&&) = delete;

    bool takeDamage(int damageAmount); ///< @brief 处理伤害逻辑，返回是否造成伤害

    // --- Setters and Getters ---
    engine::component::TransformComponent* transformComponent() const
    {
        return m_transformComponent;
    }
    engine::component::PhysicsComponent* physicsComponent() const { return m_physicsComponent; }
    engine::component::SpriteComponent* spriteComponent() const { return m_spriteComponent; }
    engine::component::AnimationComponent* animationComponent() const
    {
        return m_animationComponent;
    }
    engine::component::AudioComponent* audioComponent() const { return m_audioComponent; }
    ///< @brief 检查对象是否存活
    bool isAlive() const;

    ///< @brief 设置当前 AI 行为策略
    void setBehavior(std::unique_ptr<ai::AiBehaviorBase> behavior);

protected:
    // 核心循环方法
    void init() override;
    void update(float deltaTime, engine::core::Context& context) override;

private:
    ///< @brief 当前 AI 行为策略
    std::unique_ptr<ai::AiBehaviorBase> m_currentBehavior{ nullptr };
    /* 未来可添加一些敌人属性 */

    // --- 缓存组件指针 ---
    engine::component::TransformComponent* m_transformComponent{ nullptr };
    engine::component::PhysicsComponent* m_physicsComponent{ nullptr };
    engine::component::SpriteComponent* m_spriteComponent{ nullptr };
    engine::component::AnimationComponent* m_animationComponent{ nullptr };
    engine::component::AudioComponent* m_audioComponent{ nullptr };
};

} // namespace game::component
