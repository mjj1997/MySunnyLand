#include "game_state.h"

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <spdlog/spdlog.h>

namespace engine::core {

GameState::GameState(SDL_Window* window, SDL_Renderer* sdlRenderer, State initialState)
    : m_window{ window }
    , m_sdlRenderer{ sdlRenderer }
    , m_currentState{ initialState }
{
    if (m_window == nullptr || m_sdlRenderer == nullptr) {
        spdlog::error("窗口或渲染器为空");
        throw std::runtime_error("窗口或渲染器不能为空");
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

glm::vec2 GameState::windowSize() const
{
    int width{ 0 };
    int height{ 0 };
    // SDL3 获取窗口大小的方法
    SDL_GetWindowSize(m_window, &width, &height);
    return glm::vec2{ width, height };
}

void GameState::setWindowSize(const glm::vec2& windowSize)
{
    // SDL3 设置窗口大小的方法
    SDL_SetWindowSize(m_window, static_cast<int>(windowSize.x), static_cast<int>(windowSize.y));
    spdlog::debug("窗口大小设置为 {} x {}", windowSize.x, windowSize.y);
}

glm::vec2 GameState::logicalSize() const
{
    int width{ 0 };
    int height{ 0 };
    // SDL3 获取逻辑分辨率大小的方法
    SDL_GetRenderLogicalPresentation(m_sdlRenderer, &width, &height, nullptr);
    return glm::vec2{ width, height };
}

void GameState::setLogicalSize(const glm::vec2& logicalSize)
{
    // SDL3 设置逻辑分辨率大小的方法
    SDL_SetRenderLogicalPresentation(m_sdlRenderer,
                                     static_cast<int>(logicalSize.x),
                                     static_cast<int>(logicalSize.y),
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);
    spdlog::debug("逻辑分辨率大小设置为 {} x {}", logicalSize.x, logicalSize.y);
}

} // namespace engine::core
