#pragma once

#include "component_base.h"

namespace engine::component {

/**
 * @brief 音频组件，用于处理音频播放和管理。
 */
class AudioComponent final : public ComponentBase
{
    friend class engine::object::GameObject;

public:
    ~AudioComponent() override = default;

    // 禁止拷贝和移动
    AudioComponent(const AudioComponent&) = delete;
    AudioComponent& operator=(const AudioComponent&) = delete;
    AudioComponent(AudioComponent&&) = delete;
    AudioComponent& operator=(AudioComponent&&) = delete;

protected:
    // 核心循环方法
    void init() override;
    void update(float deltaTime, engine::core::Context& context) override {}
};

} // namespace engine::component
