#include "game_state.h"

#include <spdlog/spdlog.h>

namespace engine::core {

GameState::GameState(SDL_Window* window, State initialState)
    : m_window{ window }
    , m_currentState{ initialState }
{
    if (m_window == nullptr) {
        spdlog::error("窗口为空");
        throw std::runtime_error("窗口不能为空");
    }
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
