#pragma once

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
    explicit UiElementBase(const glm::vec2& localPosition = { 0.0f, 0.0f },
                           const glm::vec2& size = { 0.0f, 0.0f });

    ///< @brief 虚析构函数，确保派生类正确清理
    virtual ~UiElementBase() = default;

    // --- 禁用拷贝和移动语义 ---
    UiElementBase(const UiElementBase&) = delete;
    UiElementBase& operator=(const UiElementBase&) = delete;
    UiElementBase(UiElementBase&&) = delete;
    UiElementBase& operator=(UiElementBase&&) = delete;

    // --- Getters and Setters ---
    ///< @brief 获取元素位置(相对于父元素)
    const glm::vec2& localPosition() const { return m_localPosition; }
    ///< @brief 获取元素大小
    const glm::vec2& size() const { return m_size; }

    ///< @brief 设置元素位置(相对于父元素)
    void setLocalPosition(const glm::vec2& localPosition) { m_localPosition = localPosition; }
    void setSize(const glm::vec2& size) { m_size = size; }   ///< @brief 设置元素大小

protected:
    glm::vec2 m_localPosition;    ///< @brief 相对于父元素的局部位置
    glm::vec2 m_size;             ///< @brief 元素大小
};

} // namespace engine::ui
