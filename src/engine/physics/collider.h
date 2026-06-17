#pragma once

#include <glm/vec2.hpp>

namespace engine::physics {

/**
 * @brief 定义不同类型的碰撞器。
 */
enum class ColliderType {
    None,
    Aabb,
    Circle,
    // 未来可能添加其他类型，如 Capsule, Polygon 等
};

/**
 * @brief 碰撞器的抽象基类。
 * 所有具体的碰撞器都应继承此类。
 */
class ColliderBase
{
public:
    virtual ~ColliderBase() = default;

    virtual ColliderType type() const = 0; ///< @brief 获取碰撞器的类型。

    ///< @brief 设置最小包围盒的尺寸（宽度和高度）。
    void setAabbSize(glm::vec2 size) { m_aabbSize = std::move(size); }
    ///< @brief 获取最小包围盒的尺寸（宽度和高度）。
    const glm::vec2& aabbSize() const { return m_aabbSize; }

protected:
    ///< @brief 覆盖 Collider 的最小包围盒的尺寸（宽度和高度）。
    glm::vec2 m_aabbSize{ 0.0f, 0.0f };
};

/**
 * @brief 轴对齐包围盒 (Axis-Aligned Bounding Box) 碰撞器。
 */
class AabbCollider final : public ColliderBase
{
public:
    /**
     * @brief 构造函数。
     * @param size 包围盒的宽度和高度。
     */
    explicit AabbCollider(glm::vec2 size)
        : m_size{ std::move(size) }
    {
        setAabbSize(m_size);
    }
    ~AabbCollider() override = default;

    ColliderType type() const override { return ColliderType::Aabb; }

    // --- Getters and Setters ---
    const glm::vec2& size() const { return m_size; }
    void setSize(glm::vec2 size) { m_size = std::move(size); }

private:
    ///< @brief 包围盒的尺寸（和 m_aabbSize相同）。
    glm::vec2 m_size{ 0.0f, 0.0f };
};

/**
 * @brief 圆形碰撞器。
 */
class CircleCollider final : public ColliderBase
{
public:
    /**
     * @brief 构造函数。
     * @param radius 圆的半径。
     */
    explicit CircleCollider(float radius)
        : m_radius{ radius }
    {
        setAabbSize(glm::vec2{ radius * 2.0f, radius * 2.0f });
    }
    ~CircleCollider() override = default;

    ColliderType type() const override { return ColliderType::Circle; }

    // --- Getters and Setters ---
    float radius() const { return m_radius; }
    void setRadius(float radius) { m_radius = radius; }

private:
    ///< @brief 圆的半径。
    float m_radius{ 0.0f };
};

} // namespace engine::physics
