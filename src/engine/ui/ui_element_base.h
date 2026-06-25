#pragma once

#include "../utils/math.h"

#include <memory>
#include <vector>

namespace engine::core {
class Context;
}

namespace engine::ui {

/**
 * @brief 所有 UI 元素的基类
 *
 * 定义了位置、大小、可见性、状态等通用属性。
 * 管理子元素的层次结构。
 * 提供事件处理、更新和渲染的虚方法。
 */
class UiElementBase
{
public:
    /**
     * @brief 构造 UiElementBase
     * @param localPosition 初始局部位置
     * @param size 初始大小
     */
    explicit UiElementBase(glm::vec2 localPosition = { 0.0F, 0.0F },
                           glm::vec2 size = { 0.0F, 0.0F });

    ///< @brief 虚析构函数，确保派生类正确清理
    virtual ~UiElementBase() = default;

    // --- 禁用拷贝和移动语义 ---
    UiElementBase(const UiElementBase&) = delete;
    UiElementBase& operator=(const UiElementBase&) = delete;
    UiElementBase(UiElementBase&&) = delete;
    UiElementBase& operator=(UiElementBase&&) = delete;

    // --- 核心虚循环方法 --- (没有使用init和clean，注意构造函数和析构函数的使用)
    virtual bool handleInput(engine::core::Context& context);
    virtual void update(float deltaTime, engine::core::Context& context);
    virtual void render(engine::core::Context& context);

    // --- 层次结构管理 ---
    ///< @brief 添加子元素
    void addChild(std::unique_ptr<UiElementBase> child);
    ///< @brief 将指定子元素从列表中移除，并返回其智能指针
    std::unique_ptr<UiElementBase> removeChild(UiElementBase* child);
    ///< @brief 移除所有子元素
    void removeAllChildren();

    // --- Getters and Setters ---
    ///< @brief 获取元素位置(相对于父元素)
    const glm::vec2& localPosition() const { return m_localPosition; }
    ///< @brief 获取元素大小
    const glm::vec2& size() const { return m_size; }
    ///< @brief 检查元素是否可见
    bool isVisible() const { return m_isVisible; }
    ///< @brief 检查元素是否需要移除
    bool shouldRemove() const { return m_shouldRemove; }
    ///< @brief 获取父元素
    UiElementBase* parent() const { return m_parent; }
    ///< @brief 获取子元素列表
    const std::vector<std::unique_ptr<UiElementBase>>& children() const { return m_children; }

    ///< @brief 设置元素位置(相对于父元素)
    void setLocalPosition(glm::vec2 localPosition) { m_localPosition = std::move(localPosition); }
    void setSize(glm::vec2 size) { m_size = std::move(size); } ///< @brief 设置元素大小
    void setVisible(bool visible) { m_isVisible = visible; }   ///< @brief 设置元素的可见性
    ///< @brief 设置元素是否需要移除
    void setShouldRemove(bool shouldRemove) { m_shouldRemove = shouldRemove; }
    void setParent(UiElementBase* parent) { m_parent = parent; } ///< @brief 设置父元素

    // --- 辅助方法 ---
    glm::vec2 screenPosition() const;                 ///< @brief 获取(计算)元素在屏幕上的位置
    engine::utils::Rect bounds() const;               ///< @brief 获取(计算)元素的边界(屏幕坐标)
    bool isPointInside(const glm::vec2& point) const; ///< @brief 检查给定点是否在元素的边界内

protected:
    glm::vec2 m_localPosition;    ///< @brief 相对于父元素的局部位置
    glm::vec2 m_size;             ///< @brief 元素大小
    bool m_isVisible{ true };     ///< @brief 元素当前是否可见
    bool m_shouldRemove{ false }; ///< @brief 是否需要移除(延迟删除)

    UiElementBase* m_parent{ nullptr };                     ///< @brief 指向父元素的非拥有指针
    std::vector<std::unique_ptr<UiElementBase>> m_children; ///< @brief 子元素列表(容器)
};

} // namespace engine::ui
