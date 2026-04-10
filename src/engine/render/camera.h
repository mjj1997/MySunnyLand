#pragma once

#include "../utils/math.h"

#include <optional>

namespace engine::render {

/**
 * @brief 相机类负责管理相机位置和视口大小，并提供坐标转换功能。
 * 它还包含限制相机移动范围的边界。
 */
class Camera final
{
public:
    explicit Camera(const glm::vec2& viewportSize,
                    const glm::vec2& position = glm::vec2(0.0f, 0.0f),
                    const std::optional<engine::utils::Rect> limitBounds = std::nullopt);

    // 禁用拷贝和移动语义
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;
    Camera(Camera&&) = delete;
    Camera& operator=(Camera&&) = delete;

    void update(double deltaTime);      ///< @brief 更新相机位置
    void move(const glm::vec2& offset); ///< @brief 移动相机

    glm::vec2 worldToScreen(const glm::vec2& worldPos) const; ///< @brief 世界坐标转屏幕坐标
    ///< @brief 世界坐标转屏幕坐标，考虑视差滚动
    glm::vec2 worldToScreenWithParallax(const glm::vec2& worldPos,
                                        const glm::vec2& scrollFactor) const;
    glm::vec2 screenToWorld(const glm::vec2& screenPos) const; ///< @brief 屏幕坐标转世界坐标

    // --- getters and setters ---
    glm::vec2 viewportSize() const;                         ///< @brief 获取视口大小
    const glm::vec2& position() const;                      ///< @brief 获取相机位置
    std::optional<engine::utils::Rect> limitBounds() const; ///< @brief 获取限制相机的移动范围

    void setPosition(const glm::vec2& position);            ///< @brief 设置相机位置
    void setLimitBounds(const engine::utils::Rect& bounds); ///< @brief 设置限制相机的移动范围

private:
    void clampPosition(); ///< @brief 限制相机位置在边界内

private:
    glm::vec2 m_viewportSize;                         ///< @brief 视口大小（屏幕大小）
    glm::vec2 m_position;                             ///< @brief 相机左上角的世界坐标
    std::optional<engine::utils::Rect> m_limitBounds; ///< @brief 限制相机的移动范围，空值表示不限制
};

} // namespace engine::render
