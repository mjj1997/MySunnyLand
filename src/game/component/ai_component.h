#pragma once

#include "../../engine/component/component_base.h"

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

protected:
    // 核心循环方法
    void init() override;
    void update(float deltaTime, engine::core::Context& context) override;
};

} // namespace game::component
