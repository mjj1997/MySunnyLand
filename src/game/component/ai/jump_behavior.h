#pragma once

#include "ai_behavior_base.h"

namespace game::component::ai {

/**
 * @brief AI 行为：在指定范围内周期性地跳跃。
 *
 * 在地面时等待，然后向当前方向跳跃。
 * 撞墙或到达边界时改变下次跳跃方向。
 */
class JumpBehavior final : public AiBehaviorBase
{
    friend class game::component::AiComponent;

public:
    ~JumpBehavior() override = default;

    // 禁止拷贝和移动
    JumpBehavior(const JumpBehavior&) = delete;
    JumpBehavior& operator=(const JumpBehavior&) = delete;
    JumpBehavior(JumpBehavior&&) = delete;
    JumpBehavior& operator=(JumpBehavior&&) = delete;

protected:
    void update(float deltaTime, AiComponent& aiComponent) override;
};

} // namespace game::component::ai
