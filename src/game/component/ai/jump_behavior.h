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
    /**
     * @brief 构造函数。
     * @param minX 巡逻范围的最小 x 坐标。
     * @param maxX 巡逻范围的最大 x 坐标。
     * @param jumpVelocity 跳跃速度向量 (水平, 垂直)。
     * @param jumpInterval 两次跳跃之间的间隔时间。
     */
    JumpBehavior(float minX,
                 float maxX,
                 glm::vec2 jumpVelocity = glm::vec2{ 100.0F, -300.0F },
                 float jumpInterval = 2.0F);

protected:
    void update(float deltaTime, AiComponent& aiComponent) override;

private:
    float m_patrolMinX{ 0.0F };                  ///< @brief 巡逻范围的左边界
    float m_patrolMaxX{ 0.0F };                  ///< @brief 巡逻范围的右边界
    glm::vec2 m_jumpVelocity{ 100.0F, -300.0F }; ///< @brief 跳跃速度
    float m_jumpInterval{ 2.0F };                ///< @brief 跳跃间隔时间 (秒)

    float m_jumpTimer{ 0.0F };    ///< @brief 距离下次跳跃的计时器
    bool m_jumpingRight{ false }; ///< @brief 当前是否向右跳跃
};

} // namespace game::component::ai
