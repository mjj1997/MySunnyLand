#pragma once

#include "component_base.h"

namespace engine::component {

/**
 * @brief 管理 GameObject 的生命值，处理伤害、治疗，并提供无敌帧功能。
 */
class HealthComponent final : public ComponentBase
{
    friend class engine::object::GameObject;

public:
    ~HealthComponent() override = default;

    // 禁止拷贝和移动
    HealthComponent(const HealthComponent&) = delete;
    HealthComponent& operator=(const HealthComponent&) = delete;
    HealthComponent(HealthComponent&&) = delete;
    HealthComponent& operator=(HealthComponent&&) = delete;

protected:
    // 核心循环函数
    void update(float deltaTime, engine::core::Context& context) override;

};

} // namespace engine::component
