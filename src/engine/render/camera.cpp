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
