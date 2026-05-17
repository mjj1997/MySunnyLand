#include "player_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/object/game_object.h"
#include "state/player_state_base.h"
// #include "state/idle_state.h"

#include <spdlog/spdlog.h>

namespace game::component {

void PlayerComponent::setState(std::unique_ptr<state::PlayerStateBase> newState)
{
    if (!newState) {
        spdlog::warn("正在尝试设置玩家状态为空！");
        return;
    }

    if (m_currentState) {
        m_currentState->exit();
    }

    m_currentState = std::move(newState);
    spdlog::debug("PlayerComponent 正在切换到状态：{}。", typeid(*m_currentState).name());
    m_currentState->enter();
}

void PlayerComponent::init()
{
    if (!m_owner) {
        spdlog::error("PlayerComponent 没有所属游戏对象!");
        return;
    }

    // 获取必要的组件
    m_transformComponent = m_owner->getComponent<engine::component::TransformComponent>();
    m_physicsComponent = m_owner->getComponent<engine::component::PhysicsComponent>();
    m_spriteComponent = m_owner->getComponent<engine::component::SpriteComponent>();

    // 检查必要组件是否存在
    if (!m_transformComponent || !m_physicsComponent || !m_spriteComponent) {
        spdlog::error("Player 对象缺少必要组件！");
        return;
    }

    // 初始化状态机
    // m_currentState = std::make_unique<state::IdleState>(this);
    if (m_currentState) {
        setState(std::move(m_currentState));
    } else {
        spdlog::error("初始化玩家状态失败（make_unique 返回空指针）！");
    }

    spdlog::debug("PlayerComponent 初始化完成。");
}

void PlayerComponent::handleInput(engine::core::Context& context)
{
    if (!m_currentState) {
        return;
    }

    auto nextState = m_currentState->handleInput(context);
    if (nextState) {
        setState(std::move(nextState));
    }
}

void PlayerComponent::update(float deltaTime, engine::core::Context& context)
{
    if (!m_currentState) {
        return;
    }

    auto nextState = m_currentState->update(deltaTime, context);
    if (nextState) {
        setState(std::move(nextState));
    }
}

} // namespace game::component
