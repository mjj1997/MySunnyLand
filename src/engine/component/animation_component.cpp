#include "animation_component.h"
#include "../object/game_object.h"
#include "../render/animation.h"
#include "sprite_component.h"

#include <spdlog/spdlog.h>

namespace engine::component {

void AnimationComponent::addAnimation(std::unique_ptr<engine::render::Animation> animation)
{
    if (!animation) {
        return;
    }

    std::string_view name{ animation->name() }; // 获取名称
    m_animations.emplace(name, std::move(animation));
    spdlog::debug("已将动画 '{}' 添加到 GameObject '{}'",
                  name,
                  m_owner != nullptr ? m_owner->name() : "未知");
}

void AnimationComponent::playAnimation(std::string_view name)
{
    auto iter = m_animations.find(name);
    if (iter == m_animations.end() || !iter->second) {
        spdlog::warn("未找到 GameObject '{}' 的动画 '{}'",
                     m_owner != nullptr ? m_owner->name() : "未知",
                     name);
        return;
    }

    // 如果已经在播放相同的动画，不重新开始（注释这一段则重新开始播放）
    if (m_currentAnimation == iter->second.get() && m_isPlaying) {
        return;
    }

    m_currentAnimation = iter->second.get();
    m_animationTimer = 0.0F;
    m_isPlaying = true;

    // 立即将精灵更新到第一帧
    if (m_spriteComponent != nullptr && !m_currentAnimation->isEmpty()) {
        const auto& firstFrame = m_currentAnimation->frameAt(0.0F);
        m_spriteComponent->setSourceRect(firstFrame.sourceRect);
        spdlog::debug("GameObject '{}' 播放动画 '{}'",
                      m_owner != nullptr ? m_owner->name() : "未知",
                      name);
    }
}

std::string_view AnimationComponent::currentAnimationName() const
{
    if (m_currentAnimation != nullptr) {
        return m_currentAnimation->name();
    }

    return {};
}

bool AnimationComponent::isAnimationFinished() const
{
    // 如果没有当前动画(说明从未调用过playAnimation)，或者当前动画是循环的，则返回 false
    if (m_currentAnimation == nullptr || m_currentAnimation->isLoop()) {
        return false;
    }

    return m_animationTimer >= m_currentAnimation->totalDuration();
}

void AnimationComponent::init()
{
    if (m_owner == nullptr) {
        spdlog::error("AnimationComponent 没有所有者 GameObject!");
        return;
    }

    m_spriteComponent = m_owner->getComponent<SpriteComponent>();
    if (m_spriteComponent == nullptr) {
        spdlog::error("GameObject '{}' 的 AnimationComponent 需要 SpriteComponent，但没有找到!",
                      m_owner->name());
        return;
    }
}

void AnimationComponent::update(float deltaTime, engine::core::Context& /*context*/)
{
    // 如果没有正在播放的动画，或者没有当前动画，或者没有精灵组件，或者当前动画没有帧，则直接返回
    if (!m_isPlaying || m_currentAnimation == nullptr || m_spriteComponent == nullptr
        || m_currentAnimation->isEmpty()) {
        spdlog::trace("AnimationComponent 更新时没有正在播放的动画或精灵组件为空。");
        return;
    }

    // 推进计时器
    m_animationTimer += deltaTime;

    // 根据时间获取当前帧
    const auto& currentFrame = m_currentAnimation->frameAt(m_animationTimer);

    // 更新精灵组件的源矩形 (使用 SpriteComponent 的新方法)
    m_spriteComponent->setSourceRect(currentFrame.sourceRect);

    // 检查非循环动画是否已结束
    if (!m_currentAnimation->isLoop() && m_animationTimer >= m_currentAnimation->totalDuration()) {
        m_isPlaying = false;
        m_animationTimer = m_currentAnimation->totalDuration(); // 将时间限制在结束点
        if (m_isOneShotRemoval) { // 如果 isOneShotRemoval 为 true，则删除整个 GameObject
            m_owner->setShouldRemove(true);
        }
    }
}

} // namespace engine::component
