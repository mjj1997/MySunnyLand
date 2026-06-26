#include "animation.h"

#include <glm/common.hpp>
#include <spdlog/spdlog.h>

namespace engine::render {

Animation::Animation(std::string_view name, bool isLoop)
    : m_name{ name }
    , m_isLoop{ isLoop }
{}

void Animation::addFrame(SDL_FRect sourceRect, float duration)
{
    if (duration <= 0.0F) {
        spdlog::warn("尝试向动画 '{}' 添加无效持续时间的帧", m_name);
        return;
    }

    m_frames.push_back(AnimationFrame{ .sourceRect = sourceRect, .duration = duration });
    m_totalDuration += duration;
}

const AnimationFrame& Animation::frameAt(float time) const
{
    if (m_frames.empty()) {
        spdlog::error("动画 '{}' 没有帧，无法获取帧", m_name);
        return s_emptyAnimationFrame; // 返回空帧
    }

    float currentTime{ time };
    if (m_isLoop && m_totalDuration > 0.0F) {
        // 对循环动画使用模运算获取有效时间
        currentTime = glm::mod(time, m_totalDuration);
    } else {
        // 对于非循环动画，如果时间超过总时长，则停留在最后一帧
        if (currentTime >= m_totalDuration) {
            return m_frames.back();
        }
    }

    // 遍历帧以找到正确的帧
    float accumulatedTime{ 0.0F };
    for (const auto& frame : m_frames) {
        accumulatedTime += frame.duration;
        if (currentTime < accumulatedTime) {
            return frame;
        }
    }

    // 理论上在不应到达这里，但为了安全起见，返回最后一帧
    spdlog::warn("动画 '{}' 在获取帧信息时出现错误。", m_name);
    return m_frames.back();
}

} // namespace engine::render
