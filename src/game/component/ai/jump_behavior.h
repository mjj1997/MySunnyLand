#pragma once

#include "ai_behavior_base.h"

#include <glm/vec2.hpp>

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

private:
    float m_patrolMinX{ 0.0f };                  ///< @brief 巡逻范围的左边界
    float m_patrolMaxX{ 0.0f };                  ///< @brief 巡逻范围的右边界
    glm::vec2 m_jumpVelocity{ 100.0f, -300.0f }; ///< @brief 跳跃速度
    float m_jumpInterval{ 2.0f };                ///< @brief 跳跃间隔时间 (秒)

    float m_jumpTimer{ 0.0f };    ///< @brief 距离下次跳跃的计时器
};

} // namespace game::component::ai
