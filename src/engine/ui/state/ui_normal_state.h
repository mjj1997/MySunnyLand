#pragma once

#include "ui_state_base.h"

namespace engine::ui::state {

/**
 * @brief 正常状态
 *
 * 正常状态是可交互 UI 元素的默认状态。
 */
class UiNormalState final : public UiStateBase
{
    friend class engine::ui::UiInteractiveElementBase;

public:
    explicit UiNormalState(engine::ui::UiInteractiveElementBase* owner)
        : UiStateBase{ owner }
    {}

private:
    void enter() override;
    std::unique_ptr<UiStateBase> handleInput(engine::core::Context& context) override;
};

} // namespace engine::ui::state
