#pragma once

#include <memory>

namespace engine::core {
class Context;
}

namespace game::component {
class PlayerComponent;
}

namespace game::component::state {

/**
 * @brief 玩家状态机的抽象基类。
 */
class PlayerStateBase
{
    friend class game::component::PlayerComponent;

public:
    explicit PlayerStateBase(PlayerComponent* playerComponent)
        : m_playerComponent(playerComponent)
    {}
    virtual ~PlayerStateBase() = default;

    // 禁止拷贝和移动
    PlayerStateBase(const PlayerStateBase&) = delete;
    PlayerStateBase& operator=(const PlayerStateBase&) = delete;
    PlayerStateBase(PlayerStateBase&&) = delete;
    PlayerStateBase& operator=(PlayerStateBase&&) = delete;

protected:
    // --- 核心状态方法 ---
    ///< @brief 进入
    virtual void enter() = 0;
    /* handleInput 和 update 返回值为下一个状态，如果不需要切换状态，则返回 nullptr */
    ///< @brief 处理输入
    virtual std::unique_ptr<PlayerStateBase> handleInput(engine::core::Context& context) = 0;
    ///< @brief 更新
    virtual std::unique_ptr<PlayerStateBase> update(float deltaTime,
                                                    engine::core::Context& context) = 0;
    ///< @brief 离开
    virtual void exit() = 0;

    ///< @brief 指向拥有此状态的玩家组件
    PlayerComponent* m_playerComponent{ nullptr };
};

} // namespace game::component::state
