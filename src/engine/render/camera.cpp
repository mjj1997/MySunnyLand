#include "camera.h"
#include "../component/transform_component.h"

#include <spdlog/spdlog.h>

namespace engine::render {

Camera::Camera(glm::vec2 viewportSize,
               glm::vec2 position,
               std::optional<engine::utils::Rect> limitBounds)
    : m_viewportSize{ viewportSize }
    , m_position{ position }
    , m_limitBounds{ limitBounds }
{
    spdlog::trace("Camera 初始化成功，位置: {},{}", m_position.x, m_position.y);
}

void Camera::update(float deltaTime)
{
    if (m_target == nullptr) {
        return;
    }

    const glm::vec2 targetPosition{ m_target->position() };
    // 计算想要的相机位置，使目标在视口中心
    const glm::vec2 desiredCameraPosition{ targetPosition - m_viewportSize / 2.0F };

    // 计算相机当前位置和想要的相机位置之间的距离
    auto distance = glm::distance(m_position, desiredCameraPosition);
    constexpr float threshold{ 1.0F }; // 距离阈值
    if (distance < threshold) {
        // 如果相机当前位置和想要的相机位置之间的距离小于阈值，直接设置为想要的相机位置
        m_position = desiredCameraPosition;
    } else {
        // 否则，使用线性插值平滑移动到想要的相机位置
        m_position = glm::mix(m_position, desiredCameraPosition, deltaTime * m_smoothSpeed);
        m_position = glm::round(m_position); // 四舍五入到最近的整数, 省略的话会出现画面撕裂
    }

    clampPosition();
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

const glm::vec2& Camera::viewportSize() const
{
    return m_viewportSize;
}

const glm::vec2& Camera::position() const
{
    return m_position;
}

const std::optional<engine::utils::Rect>& Camera::limitBounds() const
{
    return m_limitBounds;
}

const engine::component::TransformComponent* Camera::target() const
{
    return m_target;
}

void Camera::setPosition(glm::vec2 position)
{
    m_position = position;
    clampPosition();
}

void Camera::setLimitBounds(std::optional<engine::utils::Rect> limitBounds)
{
    m_limitBounds = limitBounds;
    clampPosition();
}

void Camera::setTarget(engine::component::TransformComponent* target)
{
    m_target = target;
}

void Camera::clampPosition()
{
    // 边界检查需要确保相机视图（position 到 position + viewportSize）在 limitBounds 内
    if (m_limitBounds.has_value() && m_limitBounds.value().size.x > 0
        && m_limitBounds.value().size.y > 0) {
        // 计算允许的相机位置范围
        const glm::vec2 minCameraPos{ m_limitBounds.value().position };
        glm::vec2 maxCameraPos{ m_limitBounds.value().position + m_limitBounds.value().size
                                - m_viewportSize };

        // 确保 maxCameraPos 不小于 minCameraPos (视口可能比世界还大)
        maxCameraPos.x = std::max(minCameraPos.x, maxCameraPos.x);
        maxCameraPos.y = std::max(minCameraPos.y, maxCameraPos.y);

        m_position = glm::clamp(m_position, minCameraPos, maxCameraPos);
    }
    // 如果 limitBounds 无效则不进行限制
}

} // namespace engine::render
