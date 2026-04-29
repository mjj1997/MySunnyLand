#pragma once

#include "../physics/collider.h"
#include "../utils/alignment.h"
#include "../utils/math.h"
#include "component_base.h"

#include <memory>

namespace engine::component {
class TransformComponent;

/**
 * @brief 碰撞器组件。
 *
 * 持有 Collider 对象（如 AABBCollider）并提供获取世界坐标系下碰撞形状的方法。
 */
class ColliderComponent final : public ComponentBase
{
    friend class engine::object::GameObject;

public:
    /**
     * @brief 构造函数。
     * @param collider 指向 ColliderBase 实例的 unique_ptr，所有权将被转移。
     * @param alignment 初始的对齐锚点。
     * @param isTrigger 此碰撞器是否为触发器。
     * @param isActive 此碰撞器是否激活。
     */
    explicit ColliderComponent(std::unique_ptr<engine::physics::ColliderBase> collider,
                               engine::utils::Alignment alignment = engine::utils::Alignment::None,
                               bool isTrigger = false,
                               bool isActive = true);

    /**
     * @brief 根据当前的 alignment 和 collider 尺寸计算 offset。
     * @note 需要用到 TransformComponent 的 scale，因此 TransformComponent 更新 scale 时，也要调用此方法。
     */
    void updateOffset();

    engine::utils::Rect worldAabb() const; ///< @brief 获取世界坐标系下的最小轴对齐包围盒（AABB）。

    // --- Getters & Setters ---
    ///< @brief 获取缓存的 TransformComponent
    const TransformComponent* transformComponent() const { return m_transformComponent; }
    ///< @brief 获取 Collider 对象。
    const engine::physics::ColliderBase* collider() const { return m_collider.get(); }
    ///< @brief 获取当前计算出的偏移量。
    const glm::vec2& offset() const { return m_offset; }
    ///< @brief 获取设置的对齐锚点。
    engine::utils::Alignment alignment() const { return m_alignment; }
    bool isTrigger() const { return m_isTrigger; } ///< @brief 检查此碰撞器是否为触发器。
    bool isActive() const { return m_isActive; }   ///< @brief 检查此碰撞器是否激活。

    void setOffset(const glm::vec2& offset) { m_offset = offset; } ///< @brief 设置偏移量。
    ///< @brief 设置新的对齐方式并重新计算偏移量。
    void setAlignment(engine::utils::Alignment anchor);
    ///< @brief 设置此碰撞器是否为触发器。
    void setTrigger(bool isTrigger) { m_isTrigger = isTrigger; }
    void setActive(bool isActive) { m_isActive = isActive; } ///< @brief 设置此碰撞器是否激活。

protected:
    // 核心循环方法
    void init() override;
    void update(float, engine::core::Context&) override {}

private:
    ///< @brief 缓存的 TransformComponent 指针 (非拥有)
    TransformComponent* m_transformComponent{ nullptr };

    std::unique_ptr<engine::physics::ColliderBase> m_collider; ///< @brief 拥有的碰撞器对象。
    ///< @brief 碰撞器(最小包围盒的)左上角相对于变换原点的偏移量。
    glm::vec2 m_offset{ 0.0f, 0.0f };
    engine::utils::Alignment m_alignment{ engine::utils::Alignment::None }; ///< @brief 对齐方式。

    bool m_isTrigger{ false }; ///< @brief 是否为触发器 (仅检测碰撞，不产生物理响应)
    bool m_isActive{ true };   ///< @brief 是否激活
};

} // namespace engine::component
