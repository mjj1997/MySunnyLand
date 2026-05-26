#include "jump_behavior.h"
#include "../ai_component.h"

#include "../../../engine/component/animation_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include "../../../engine/component/transform_component.h"

#include <spdlog/spdlog.h>

namespace game::component::ai {

JumpBehavior::JumpBehavior(float minX, float maxX, glm::vec2 jumpVelocity, float jumpInterval)
    : m_patrolMinX{ minX }
    , m_patrolMaxX{ maxX }
    , m_jumpVelocity{ jumpVelocity }
    , m_jumpInterval{ jumpInterval }
{
    if (m_patrolMinX > m_patrolMaxX) {
        spdlog::error("JumpBehavior: minX ({}) 应小于 maxX ({})。行为可能不正确。", minX, maxX);
        m_patrolMinX = m_patrolMaxX; // 修正为相等，避免逻辑错误
    }

    // 确保垂直跳跃速度是负数（向上）
    if (m_jumpVelocity.y > 0.0f) {
        spdlog::error("JumpBehavior: 垂直跳跃速度（{}）应为负数（向上）。已取相反数。",
                      m_jumpVelocity.y);
        m_jumpVelocity.y = -m_jumpVelocity.y;
    }

    // 确保跳跃间隔时间是正数
    if (m_jumpInterval <= 0.0f) {
        spdlog::error("JumpBehavior: 跳跃间隔时间（{}）应为正数。已修正为 2.0 秒。", m_jumpInterval);
        m_jumpInterval = 2.0f;
    }
}

void JumpBehavior::update(float deltaTime, AiComponent& aiComponent)
{
    // 获取必要的组件
    auto* physicsComponent = aiComponent.physicsComponent();
    auto* transformComponent = aiComponent.transformComponent();
    auto* spriteComponent = aiComponent.spriteComponent();
    auto* animationComponent = aiComponent.animationComponent();
    if (!physicsComponent || !transformComponent || !spriteComponent || !animationComponent) {
        spdlog::error("JumpBehavior：缺少必要的组件，无法执行跳跃行为。");
        return;
    }

    // 着地标志
    auto isOnGround = physicsComponent->isCollidedBelow();
    if (isOnGround) { // 如果在地面上
        // 增加跳跃计时器
        m_jumpTimer += deltaTime;
        // 停止水平移动（否则会有惯性）
        glm::vec2 newVelocity{ 0.0f, physicsComponent->velocity().y };
        physicsComponent->setVelocity(newVelocity);

        // --- 检查是否需要跳跃 ---
        if (m_jumpTimer >= m_jumpInterval) { // 时间到，准备跳跃
            // 重置计时器
            m_jumpTimer = 0.0f;

            // --- 检查是否需要更新跳跃方向 ---
            auto currentX = transformComponent->position().x;
            if (m_jumpingRight
                && (physicsComponent->isCollidedRight() || currentX >= m_patrolMaxX)) {
                // 如果右边超限或者撞墙，向左跳
                m_jumpingRight = false;
            } else if (!m_jumpingRight
                       && (physicsComponent->isCollidedLeft() || currentX <= m_patrolMinX)) {
                // 如果左边超限或者撞墙，向右跳
                m_jumpingRight = true;
            }

            // 根据跳跃方向标志，确定水平跳跃方向，设置速度
            auto jumpVelocityX = m_jumpingRight ? m_jumpVelocity.x : -m_jumpVelocity.x;
            physicsComponent->setVelocity(glm::vec2{ jumpVelocityX, m_jumpVelocity.y });
            // 播放跳跃动画
            animationComponent->playAnimation("jump");
            // 更新精灵翻转
            spriteComponent->setFlipped(m_jumpingRight);
        } else { // 还在地面等待
            animationComponent->playAnimation("idle");
        }
    } else { // 如果在空中
        // --- 根据垂直速度判断是上升(jump)还是下落(fall) ---
        if (physicsComponent->velocity().y < 0) {
            animationComponent->playAnimation("jump");
        } else {
            animationComponent->playAnimation("fall");
        }
    }
}

} // namespace game::component::ai
