#include "animation_component.h"

namespace engine::component {

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

} // namespace engine::component
