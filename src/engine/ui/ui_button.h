#pragma once

#include "ui_interactive_element_base.h"

#include <functional>

namespace engine::ui {

/**
 * @brief 可交互的按钮 UI 元素
 * 
 * 继承自 UiInteractiveElementBase，用于创建可交互的按钮。
 * 支持三种状态：正常、悬停、按下。
 * 支持回调函数，当按钮被点击时调用。
 */
class UiButton final : public UiInteractiveElementBase
{
public:
    /**
     * @brief 构造函数
     * @param context 引擎上下文
     * @param normalSpriteId 正常状态的精灵ID
     * @param hoverSpriteId 悬停状态的精灵ID
     * @param pressedSpriteId 按下状态的精灵ID
     * @param localPosition 局部位置
     * @param size 大小
     * @param callback 回调函数
     */
    UiButton(engine::core::Context& context,
             std::string_view normalSpriteId,
             std::string_view hoverSpriteId,
             std::string_view pressedSpriteId,
             glm::vec2 localPosition = { 0.0F, 0.0F },
             glm::vec2 size = { 0.0F, 0.0F },
             std::function<void()> callback = nullptr);

    ~UiButton() override = default;

    void clicked() override; ///< @brief 重写基类方法，当按钮被点击时调用回调函数

    ///< @brief 设置点击回调函数
    void setCallback(std::function<void()> callback) { m_callback = std::move(callback); }
    std::function<void()> callback() const { return m_callback; } ///< @brief 获取点击回调函数

private:
    std::function<void()> m_callback; ///< @brief 可自定义的函数（函数包装器）
};

} // namespace engine::ui
