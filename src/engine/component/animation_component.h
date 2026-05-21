#pragma once

#include "component_base.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace engine::render {
class Animation;
}

namespace engine::component {
class SpriteComponent;
}

namespace engine::component {

/**
 * @brief GameObject的动画组件。
 *
 * 持有一组 Animation 对象并控制其播放，
 * 根据当前帧更新关联的 SpriteComponent。
 */
class AnimationComponent final : public ComponentBase
{
    friend class engine::object::GameObject;

public:
    AnimationComponent() = default;

    // 删除复制/移动操作
    AnimationComponent(const AnimationComponent&) = delete;
    AnimationComponent& operator=(const AnimationComponent&) = delete;
    AnimationComponent(AnimationComponent&&) = delete;
    AnimationComponent& operator=(AnimationComponent&&) = delete;

    ///< @brief 向 m_animations map容器中添加一个动画。
    void addAnimation(std::unique_ptr<engine::render::Animation> animation);
    void playAnimation(const std::string& name);  ///< @brief 播放指定名称的动画。
    void stopAnimation() { m_isPlaying = false; } ///< @brief 停止当前动画播放。

    // --- Getters and Setters ---
    std::string currentAnimationName() const;
    bool isPlaying() const { return m_isPlaying; }
    bool isAnimationFinished() const;
    bool isOneShotRemoval() const { return m_isOneShotRemoval; }
    void setOneShotRemoval(bool isOneShotRemoval) { m_isOneShotRemoval = isOneShotRemoval; }

protected:
    // 核心循环方法
    void init() override;
    void update(float deltaTime, engine::core::Context& context) override;

private:
    /// @brief 动画名称到 Animation 对象的映射。
    std::unordered_map<std::string, std::unique_ptr<engine::render::Animation>> m_animations;
    SpriteComponent* m_spriteComponent{ nullptr }; ///< @brief 指向必需的 SpriteComponent 的指针

    engine::render::Animation* m_currentAnimation{ nullptr }; ///< @brief 指向当前播放动画的原始指针
    float m_animationTimer{ 0.0f };                           ///< @brief 动画播放中的计时器

    bool m_isPlaying{ false };        ///< @brief 当前是否有动画正在播放
    bool m_isOneShotRemoval{ false }; ///< @brief 是否在动画结束后删除整个GameObject
};

} // namespace engine::component
