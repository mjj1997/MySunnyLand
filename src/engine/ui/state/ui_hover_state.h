#pragma once

#include "ui_state_base.h"

namespace engine::ui::state {

/**
 * @brief 悬停状态
 *
 * 当鼠标悬停在可交互 UI 元素上时，会切换到该状态。
 */
class UiHoverState final : public UiStateBase
{
    friend class engine::ui::UiInteractiveElementBase;

public:
    explicit UiHoverState(engine::ui::UiInteractiveElementBase* owner)
        : UiStateBase{ owner }
    {}

private:
    void enter() override;
    std::unique_ptr<UiStateBase> handleInput(engine::core::Context& context) override;
};

} // namespace engine::ui::state
