#pragma once

#include "ui_state_base.h"

namespace engine::ui::state {

/**
 * @brief 按下状态
 *
 * 当鼠标按下可交互 UI 元素时，会切换到该状态。
 */
class UiPressedState final : public UiStateBase
{
    friend class engine::ui::UiInteractiveElementBase;

public:
    explicit UiPressedState(engine::ui::UiInteractiveElementBase* owner)
        : UiStateBase{ owner }
    {}

private:
    void enter() override;
    std::unique_ptr<UiStateBase> handleInput(engine::core::Context& context) override;
};

} // namespace engine::ui::state
