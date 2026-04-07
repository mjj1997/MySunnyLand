#pragma once

#include <SDL3/SDL_stdinc.h> // 用于 Uint64

namespace engine::core {

/**
 * @brief 管理游戏循环中的时间，计算帧间时间差（deltaTime）。
 *
 * 使用 SDL 的高精度性能计数器来确保时间测量的准确性。
 * 提供获取缩放和未缩放帧间时间差（deltaTime）的方法，以及设置时间缩放因子的能力。
 */
class FrameTimeController final
{
public:
    FrameTimeController();

    // 简单起见，直接删除拷贝、移动和赋值运算符
    FrameTimeController(const FrameTimeController&) = delete;
    FrameTimeController& operator=(const FrameTimeController&) = delete;
    FrameTimeController(FrameTimeController&&) = delete;
    FrameTimeController& operator=(FrameTimeController&&) = delete;

    /**
     * @brief 每帧开始时调用，更新内部时间状态并计算 deltaTime。
     */
    void update();

    /**
     * @brief 获取经过时间缩放调整后的帧间时间差（deltaTime）。
     *
     * @return double 缩放后的 deltaTime (秒)。
     */
    double deltaTime() const;

    /**
     * @brief 获取未经过时间缩放的原始帧间时间差。
     *
     * @return double 未缩放的 deltaTime (秒)。
     */
    double unscaledDeltaTime() const;

    /**
     * @brief 设置时间缩放因子。
     *
     * @param scale 时间缩放值。1.0 为正常速度，< 1.0 为慢动作，> 1.0 为快进。
     *              不允许负值。
     */
    void setTimeScale(double scale);

    /**
     * @brief 获取当前的时间缩放因子。
     *
     * @return double 当前的时间缩放因子。
     */
    double timeScale() const;

    /**
     * @brief 设置目标帧率。
     *
     * @param fps 目标每秒帧数。设置为 0 表示不限制帧率。负值将被视为 0。
     */
    void setTargetFps(int fps);

    /**
     * @brief 获取当前设置的目标帧率。
     *
     * @return int 目标 FPS，0 表示不限制。
     */
    int targetFps() const;

private:
    /**
     * @brief update 中调用，用于限制帧率。如果设置了 m_targetFps > 0，且当前帧执行时间小于目标帧时间，则会调用 SDL_DelayNS() 来等待剩余时间。
     * 
     * @param deltaTime 当前帧的执行时间（秒）
     */
    void limitFrameRate(double deltaTime);

private:
    Uint64 m_lastFrameEndTimestamp{ 0 };      ///< @brief 上一帧结束的时间戳
    Uint64 m_currentFrameStartTimestamp{ 0 }; ///< @brief 当前帧开始的时间戳
    double m_deltaTime{ 0.0 };                ///< @brief 未缩放的帧间时间差 (秒)
    double m_timeScale{ 1.0 };                ///< @brief 时间缩放因子

    // 帧率限制相关
    int m_targetFps{ 0 };            ///< @brief 目标 FPS (0 表示不限制)
    double m_targetFrameTime{ 0.0 }; ///< @brief 目标每帧时间 (秒)
};

} // namespace engine::core
