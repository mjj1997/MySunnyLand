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

void Camera::update(float deltaTime)
{
    // TODO: 自动跟随目标
}

void Camera::move(const glm::vec2& offset)
{
    m_position += offset;
    clampPosition();
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
    clampPosition();
}

void Camera::setLimitBounds(const engine::utils::Rect& bounds)
{
    m_limitBounds = bounds;
    clampPosition();
}

void Camera::clampPosition()
{
    // 边界检查需要确保相机视图（position 到 position + viewportSize）在 limitBounds 内
    if (m_limitBounds.has_value() && m_limitBounds.value().size.x > 0
        && m_limitBounds.value().size.y > 0) {
        // 计算允许的相机位置范围
        glm::vec2 minCameraPos = m_limitBounds.value().position;
        glm::vec2 maxCameraPos = m_limitBounds.value().position + m_limitBounds.value().size
                                 - m_viewportSize;

        // 确保 maxCameraPos 不小于 minCameraPos (视口可能比世界还大)
        maxCameraPos.x = std::max(minCameraPos.x, maxCameraPos.x);
        maxCameraPos.y = std::max(minCameraPos.y, maxCameraPos.y);

        m_position = glm::clamp(m_position, minCameraPos, maxCameraPos);
    }
    // 如果 limitBounds 无效则不进行限制
}

} // namespace engine::render
