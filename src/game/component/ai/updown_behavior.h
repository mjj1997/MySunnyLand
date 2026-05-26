#pragma once

#include "ai_behavior_base.h"

namespace game::component::ai {

/**
 * @brief AI 行为：在指定范围内上下垂直移动。
 *
 * 到达边界或碰到障碍物时会反向。
 */
class UpDownBehavior final : public AiBehaviorBase
{
    friend class game::component::AiComponent;

public:
    ~UpDownBehavior() override = default;

    // 禁止拷贝和移动
    UpDownBehavior(const UpDownBehavior&) = delete;
    UpDownBehavior& operator=(const UpDownBehavior&) = delete;
    UpDownBehavior(UpDownBehavior&&) = delete;
    UpDownBehavior& operator=(UpDownBehavior&&) = delete;

protected:
    void enter(AiComponent& aiComponent) override;
    void update(float deltaTime, AiComponent& aiComponent) override;
};

} // namespace game::component::ai
