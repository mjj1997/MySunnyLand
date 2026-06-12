#pragma once

#include "state/ui_state_base.h"
#include "ui_element_base.h"

namespace engine::ui {
/**
 * @brief 可交互 UI 元素的基类，继承自 UiElementBase
 *
 * 定义了可交互 UI 元素的通用属性和行为。
 * 管理 UI 元素状态的切换和交互逻辑。
 * 提供事件处理、更新和渲染的虚方法。
 */
class UiInteractiveElementBase : public UiElementBase
{
public:
    // --- 核心方法 ---
    bool handleInput(engine::core::Context& context) override;
    void render(engine::core::Context& context) override;

    // --- Getters and Setters ---
    ///< @brief 设置当前状态
    void setCurrentState(std::unique_ptr<engine::ui::state::UiStateBase> state);
    ///< @brief 获取当前状态
    engine::ui::state::UiStateBase* currentState() const { return m_currentState.get(); }

protected:
    std::unique_ptr<engine::ui::state::UiStateBase> m_currentState; ///< @brief 当前状态
};

} // namespace engine::ui
