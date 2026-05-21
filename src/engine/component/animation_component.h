#pragma once

#include "component_base.h"

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

protected:
    // 核心循环方法
    void update(float deltaTime, engine::core::Context& context) override;
};

} // namespace engine::component
