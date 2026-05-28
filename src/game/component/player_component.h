#pragma once

#include "../../engine/component/component_base.h"
#include "state/player_state_base.h"

#include <memory>

namespace engine::component {
class TransformComponent;
class PhysicsComponent;
class SpriteComponent;
class AnimationComponent;
class HealthComponent;
} // namespace engine::component

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

    bool takeDamage(int damageAmount); ///< @brief 试图造成伤害，返回是否成功

    // --- setters and getters ---
    engine::component::TransformComponent* transformComponent() const
    {
        return m_transformComponent;
    }
    engine::component::SpriteComponent* spriteComponent() const { return m_spriteComponent; }
    engine::component::PhysicsComponent* physicsComponent() const { return m_physicsComponent; }
    engine::component::AnimationComponent* animationComponent() const
    {
        return m_animationComponent;
    }
    engine::component::HealthComponent* healthComponent() const { return m_healthComponent; }

    void setAlive(bool isAlive) { m_isAlive = isAlive; }            ///< @brief 设置玩家是否存活
    bool isAlive() const { return m_isAlive; }                      ///< @brief 获取玩家是否存活
    void setMoveForce(float moveForce) { m_moveForce = moveForce; } ///< @brief 设置水平移动力
    float moveForce() const { return m_moveForce; }                 ///< @brief 获取水平移动力
    void setMaxSpeed(float maxSpeed) { m_maxSpeed = maxSpeed; }     ///< @brief 设置最大移动速度
    float maxSpeed() const { return m_maxSpeed; }                   ///< @brief 获取最大移动速度
    ///< @brief 设置摩擦系数
    void setFrictionFactor(float frictionFactor) { m_frictionFactor = frictionFactor; }
    float frictionFactor() const { return m_frictionFactor; } ///< @brief 获取摩擦系数
    ///< @brief 设置跳跃速度
    void setJumpVelocity(float jumpVelocity) { m_jumpVelocity = jumpVelocity; }
    float jumpVelocity() const { return m_jumpVelocity; } ///< @brief 获取跳跃速度
    ///< @brief 设置爬梯速度
    void setClimbVelocity(float climbVelocity) { m_climbVelocity = climbVelocity; }
    float climbVelocity() const { return m_climbVelocity; } ///< @brief 获取爬梯速度
    ///< @brief 设置硬直时间
    void setStunnedDuration(float duration) { m_stunnedDuration = duration; }
    float stunnedDuration() const { return m_stunnedDuration; } ///< @brief 获取硬直时间

    ///< @brief 检查玩家是否在地面上（考虑土狼时间）
    bool isOnGround() const;
    ///< @brief 切换玩家状态
    void setState(std::unique_ptr<state::PlayerStateBase> newState);

protected:
    // 核心循环函数
    void init() override;
    void handleInput(engine::core::Context& context) override;
    void update(float deltaTime, engine::core::Context& context) override;

private:
    ///< @brief 指向 TransformComponent 的非拥有指针
    engine::component::TransformComponent* m_transformComponent{ nullptr };
    ///< @brief 指向 SpriteComponent 的非拥有指针
    engine::component::SpriteComponent* m_spriteComponent{ nullptr };
    ///< @brief 指向 PhysicsComponent 的非拥有指针
    engine::component::PhysicsComponent* m_physicsComponent{ nullptr };
    ///< @brief 指向 AnimationComponent 的非拥有指针
    engine::component::AnimationComponent* m_animationComponent{ nullptr };
    ///< @brief 指向 HealthComponent 的非拥有指针
    engine::component::HealthComponent* m_healthComponent{ nullptr };

    std::unique_ptr<state::PlayerStateBase> m_currentState;
    bool m_isAlive{ true };

    // --- 移动相关参数 ---
    float m_moveForce{ 200.0f };     ///< @brief 水平移动力
    float m_maxSpeed{ 120.0f };      ///< @brief 最大移动速度 (像素/秒)
    float m_frictionFactor{ 0.85f }; ///< @brief 摩擦系数 (Idle时缓冲效果，每帧乘以此系数)
    float m_jumpVelocity{ 350.0f };  ///< @brief 跳跃速度 (按下"jump"键给的瞬间向上的速度)
    float m_climbVelocity{ 100.0f }; ///< @brief 爬梯速度 (像素/秒)

    // --- 属性相关参数 ---
    float m_stunnedDuration{ 0.4f }; ///< @brief 玩家被击中后的硬直时间（单位：秒）

    // 土狼时间（Coyote Time）：允许玩家在离地后，短暂时间内仍然可以跳跃
    static constexpr float m_coyoteTime{ 0.1f }; ///< @brief 土狼时间（单位：秒）
    float m_coyoteTimer{ 0.0f };                 ///< @brief 土狼时间计时器（单位：秒）
};

} // namespace game::component
