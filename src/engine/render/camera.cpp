#include "camera.h"

#include <spdlog/spdlog.h>

namespace engine::render {

Camera::Camera(const glm::vec2& viewportSize,
               const glm::vec2& position,
               const std::optional<engine::utils::Rect> limitBounds)
    : m_viewportSize(viewportSize)
    , m_position(position)
    , m_limitBounds(limitBounds)
{
    spdlog::trace("Camera 初始化成功，位置: {},{}", position.x, position.y);
}

glm::vec2 Camera::worldToScreen(const glm::vec2& worldPos) const
{
    // 世界坐标 - 相机位置 = 屏幕坐标
    return worldPos - m_position;
}

glm::vec2 Camera::worldToScreenWithParallax(const glm::vec2& worldPos,
                                            const glm::vec2& scrollFactor) const
{
    // 世界坐标 - 相机位置 * 滚动因子 = 屏幕坐标
    return worldPos - m_position * scrollFactor;
}

glm::vec2 Camera::screenToWorld(const glm::vec2& screenPos) const
{
    // 屏幕坐标 + 相机位置 = 世界坐标
    return screenPos + m_position;
}

glm::vec2 Camera::viewportSize() const
{
    return m_viewportSize;
}

const glm::vec2& Camera::position() const
{
    return m_position;
}

std::optional<engine::utils::Rect> Camera::limitBounds() const
{
    return m_limitBounds;
}

void Camera::setPosition(const glm::vec2& position)
{
    m_position = position;
}

void Camera::setLimitBounds(const engine::utils::Rect& bounds)
{
    m_limitBounds = bounds;
}

// TODO: 实现相机位置的边界检查
} // namespace engine::render
