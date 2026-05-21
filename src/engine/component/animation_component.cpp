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

    std::string name{ animation->name() }; // 获取名称
    m_animations[name] = std::move(animation);
    spdlog::debug("已将动画 '{}' 添加到 GameObject '{}'", name, m_owner ? m_owner->name() : "未知");
}

std::string AnimationComponent::currentAnimationName() const
{
    if (m_currentAnimation) {
        return m_currentAnimation->name();
    }

    return "";
}

bool AnimationComponent::isAnimationFinished() const
{
    // 如果没有当前动画(说明从未调用过playAnimation)，或者当前动画是循环的，则返回 false
    if (!m_currentAnimation || m_currentAnimation->isLoop()) {
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

} // namespace engine::component
