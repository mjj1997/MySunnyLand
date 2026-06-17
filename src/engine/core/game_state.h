#pragma once

#include <glm/vec2.hpp>

struct SDL_Window;
struct SDL_Renderer;

namespace engine::core {

/**
 * @enum State
 * @brief 定义游戏可能处于的宏观状态。
 */
enum class State {
    InTitle,  ///< @brief 标题界面
    Playing,  ///< @brief 正常游戏进行中
    Paused,   ///< @brief 游戏暂停（通常覆盖菜单界面）
    GameOver, ///< @brief 游戏结束界面
    // 可以根据需要添加更多状态，如 Cutscene, SettingsMenu 等
};

/**
 * @brief 管理和查询游戏的全局宏观状态。
 *
 * 提供一个中心点来确定游戏当前处于哪个主要模式，
 * 以便其他系统（输入、渲染、更新等）可以相应地调整其行为。
 */
class GameState final
{
public:
    /**
     * @brief 构造函数，初始化游戏状态。
     * @param window SDL 窗口，必须传入有效值。
     * @param sdlRenderer SDL 渲染器，必须传入有效值。
     * @param initialState 游戏的初始状态，默认为 InTitle
     */
    GameState(SDL_Window* window, SDL_Renderer* sdlRenderer, State initialState = State::InTitle);

    // --- getters & setters ---
    State currentState() const { return m_currentState; }
    void setCurrentState(State newState);
    glm::vec2 windowSize() const;
    //这里不涉及成员变量的赋值，只是访问 windowSize 的值，所以不用 std::move
    void setWindowSize(const glm::vec2& windowSize);
    glm::vec2 logicalSize() const;
    //这里不涉及成员变量的赋值，只是访问 logicalSize 的值，所以不用 std::move
    void setLogicalSize(const glm::vec2& logicalSize);

    // --- 便捷查询方法 ---
    bool isInTitle() const { return m_currentState == State::InTitle; }
    bool isPlaying() const { return m_currentState == State::Playing; }
    bool isPaused() const { return m_currentState == State::Paused; }
    bool isGameOver() const { return m_currentState == State::GameOver; }

private:
    SDL_Window* m_window{ nullptr };        ///< @brief SDL 窗口，用于获取窗口大小
    SDL_Renderer* m_sdlRenderer{ nullptr }; ///< @brief SDL 渲染器，用于获取逻辑分辨率

    State m_currentState{ State::InTitle }; ///< @brief 当前游戏状态
};

} // namespace engine::core
