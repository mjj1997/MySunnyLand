#include "frame_time_controller.h"

#include <SDL3/SDL_timer.h> // 用于 SDL_GetTicksNS()
#include <spdlog/spdlog.h>

namespace engine::core {

FrameTimeController::FrameTimeController()
{
    // 初始化 lastFrameEndTimestamp 为当前时间，避免第一帧 DeltaTime 过大
    m_lastFrameEndTimestamp = SDL_GetTicksNS();
    spdlog::trace("FrameTimeController 初始化。Last frame endtimestamp: {}",
                  m_lastFrameEndTimestamp);
}

void FrameTimeController::update()
{
    m_currentFrameStartTimestamp = SDL_GetTicksNS(); // 记录进入 update 时的时间戳
    auto deltaTime = static_cast<double>(m_currentFrameStartTimestamp - m_lastFrameEndTimestamp)
                     / 1.0e9;
    // 如果设置了目标帧率，则限制帧率；否则 m_deltaTime = deltaTime
    if (m_targetFrameTime > 0.0) {
        limitFrameRate(deltaTime);
    } else {
        m_deltaTime = deltaTime;
    }

    m_lastFrameEndTimestamp = SDL_GetTicksNS(); // 记录离开 update 时的时间戳
}

void FrameTimeController::limitFrameRate(double deltaTime)
{
    // 如果当前帧耗费的时间小于目标帧时间，则等待剩余时间
    if (deltaTime < m_targetFrameTime) {
        double s2Wait{ m_targetFrameTime - deltaTime };        // 需要等待的时间差（秒）
        Uint64 ns2Wait{ static_cast<Uint64>(s2Wait * 1.0e9) }; // 转换为纳秒
        SDL_DelayNS(ns2Wait);
        m_deltaTime = static_cast<double>(SDL_GetTicksNS() - m_lastFrameEndTimestamp) / 1.0e9;
    }
}

double FrameTimeController::deltaTime() const
{
    return m_deltaTime * m_timeScale;
}

double FrameTimeController::unscaledDeltaTime() const
{
    return m_deltaTime;
}

void FrameTimeController::setTimeScale(double scale)
{
    if (scale < 0.0) {
        spdlog::warn("Time scale 不能为负。Clamping to 0.");
        scale = 0.0; // 防止负时间缩放
    }
    m_timeScale = scale;
}

double FrameTimeController::timeScale() const
{
    return m_timeScale;
}

void FrameTimeController::setTargetFps(int fps)
{
    if (fps < 0) {
        spdlog::warn("Target FPS 不能为负。Setting to 0 (unlimited).");
        m_targetFps = 0;
    } else {
        m_targetFps = fps;
    }

    if (m_targetFps > 0) {
        m_targetFrameTime = 1.0 / static_cast<double>(m_targetFps);
        spdlog::info("Target FPS 设置为: {} (Frame time: {:.6f}s)", m_targetFps, m_targetFrameTime);
    } else {
        m_targetFrameTime = 0.0;
        spdlog::info("Target FPS 设置为: Unlimited");
    }
}

int FrameTimeController::targetFps() const
{
    return m_targetFps;
}

} // namespace engine::core
