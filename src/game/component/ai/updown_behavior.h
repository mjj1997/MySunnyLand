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

private:
    float m_patrolMinY{ 0.0f }; ///< @brief 巡逻范围的上边界 (Y 坐标较小值)
    float m_patrolMaxY{ 0.0f }; ///< @brief 巡逻范围的下边界 (Y 坐标较大值)
    float m_moveSpeed{ 50.0f }; ///< @brief 移动速度 (像素/秒)
};

} // namespace game::component::ai
