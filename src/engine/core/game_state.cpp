#include "game_state.h"

#include <spdlog/spdlog.h>

namespace engine::core {

GameState::GameState(State initialState)
    : m_currentState{ initialState }
{
    spdlog::trace("游戏状态初始化完成");
}

void GameState::setCurrentState(State newState)
{
    if (m_currentState != newState) {
        spdlog::debug("游戏状态改变");
        m_currentState = newState;
    } else {
        spdlog::debug("尝试设置相同的游戏状态，跳过");
    }
}

} // namespace engine::core
