#pragma once

#include "ai_behavior_base.h"

namespace game::component::ai {

/**
 * @brief AI 行为：在指定范围内左右巡逻。
 *
 * 遇到墙壁或到达巡逻边界时会转身。
 */
class PatrolBehavior final : public AiBehaviorBase
{
    friend class game::component::AiComponent;

public:
    ~PatrolBehavior() override = default;

    // 禁止拷贝和移动
    PatrolBehavior(const PatrolBehavior&) = delete;
    PatrolBehavior& operator=(const PatrolBehavior&) = delete;
    PatrolBehavior(PatrolBehavior&&) = delete;
    PatrolBehavior& operator=(PatrolBehavior&&) = delete;

protected:
    void enter(AiComponent& aiComponent) override;
    void update(float deltaTime, AiComponent& aiComponent) override;
};

} // namespace game::component::ai
