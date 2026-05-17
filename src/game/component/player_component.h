#pragma once

#include "../../engine/component/component_base.h"

namespace game::component {

/**
 * @brief 处理玩家输入、状态, 控制 GameObject 移动的组件。
 *        使用状态机模式管理 Idle, Walk, Jump, Fall 等状态。
 */
class PlayerComponent final : public engine::component::ComponentBase
{
    friend class engine::object::GameObject;

public:
    PlayerComponent() = default;
    ~PlayerComponent() override = default;

    // 禁止拷贝和移动
    PlayerComponent(const PlayerComponent&) = delete;
    PlayerComponent& operator=(const PlayerComponent&) = delete;
    PlayerComponent(PlayerComponent&&) = delete;
    PlayerComponent& operator=(PlayerComponent&&) = delete;

protected:
    // 核心循环函数
    void init() override;
    void handleInput(engine::core::Context& context) override;
    void update(float deltaTime, engine::core::Context& context) override;
};

} // namespace game::component
