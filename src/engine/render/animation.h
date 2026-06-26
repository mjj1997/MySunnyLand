#pragma once

#include <SDL3/SDL_rect.h>

#include <string>
#include <vector>

namespace engine::render {

/**
 * @brief 代表动画中的单个帧。
 *
 * 包含纹理图集上的源矩形和该帧的显示持续时间。
 */
struct AnimationFrame
{
    SDL_FRect sourceRect; ///< @brief 纹理图集上此帧的区域
    float duration;       ///< @brief 此帧显示的持续时间（秒）
};

/**
 * @brief 代表整个动画。管理一系列动画帧。
 *
 * 存储动画的帧、总时长、名称和循环行为。
 */
class Animation final
{
public:
    /**
     * @brief 构造函数
     * @param name 动画的名称。
     * @param isLoop 动画是否应该循环播放。
     */
    explicit Animation(std::string_view name = "default", bool isLoop = true);
    ~Animation() = default;

    // 禁止拷贝和移动，因为 Animation 通常由管理器持有，不应随意拷贝
    Animation(const Animation&) = delete;
    Animation& operator=(const Animation&) = delete;
    Animation(Animation&&) = delete;
    Animation& operator=(Animation&&) = delete;

    /**
     * @brief 向动画添加一帧。
     *
     * @param sourceRect 纹理图集上此帧的区域。
     * @param duration 此帧应显示的持续时间（秒）。
     */
    void addFrame(SDL_FRect sourceRect, float duration);

    /**
     * @brief 获取在给定时间点应该显示的动画帧。
     * @param time 当前时间（秒）。如果动画循环，则可以超过总持续时间。
     * @return 对应时间点的动画帧。
     */
    const AnimationFrame& frameAt(float time) const;

    // --- Setters and Getters ---
    std::string_view name() const { return m_name; } ///< @brief 获取动画名称。
    ///< @brief 获取动画帧列表。
    const std::vector<AnimationFrame>& frames() const { return m_frames; }
    size_t frameCount() const { return m_frames.size(); }   ///< @brief 获取帧数量。
    float totalDuration() const { return m_totalDuration; } ///< @brief 获取动画的总持续时间（秒）。
    bool isLoop() const { return m_isLoop; }                ///< @brief 检查动画是否循环播放。
    bool isEmpty() const { return m_frames.empty(); }       ///< @brief 检查动画是否没有帧。

    void setName(std::string_view name) { m_name = name; } ///< @brief 设置动画名称。
    void setLoop(bool loop) { m_isLoop = loop; }           ///< @brief 设置动画是否循环播放。

private:
    std::string m_name;                   ///< @brief 动画的名称 (例如, "walk", "idle")。
    std::vector<AnimationFrame> m_frames; ///< @brief 动画帧列表
    float m_totalDuration{ 0.0F };        ///< @brief 动画的总持续时间（秒）
    bool m_isLoop{ true };                ///< @brief 默认动画是循环的

    inline static AnimationFrame s_emptyAnimationFrame{};
};

} // namespace engine::render
