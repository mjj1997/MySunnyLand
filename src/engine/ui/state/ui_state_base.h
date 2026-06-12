#pragma once

#include <memory>

namespace engine::core {
class Context;
}

namespace engine::ui {
class UiInteractiveElementBase;
}

namespace engine::ui::state {

/**
 * @brief 可交互 UI 元素在特定状态下的行为接口。
 *
 * 该接口定义了所有具体 UI 状态必须实现的通用操作，
 * 例如处理输入事件、更新状态逻辑以及确定视觉表现。
 */
class UiStateBase
{
    friend class engine::ui::UiInteractiveElementBase;

public:
    /**
     * @brief 构造函数传入状态拥有者指针
     */
    UiStateBase(engine::ui::UiInteractiveElementBase* owner)
        : m_owner{ owner }
    {}

    virtual ~UiStateBase() = default;

    // 删除拷贝和移动构造函数/赋值运算符
    UiStateBase(const UiStateBase&) = delete;
    UiStateBase& operator=(const UiStateBase&) = delete;
    UiStateBase(UiStateBase&&) = delete;
    UiStateBase& operator=(UiStateBase&&) = delete;

protected:
    // --- 核心方法 ---
    virtual void enter() = 0;
    virtual std::unique_ptr<UiStateBase> handleInput(engine::core::Context& context) = 0;

    engine::ui::UiInteractiveElementBase* m_owner{ nullptr }; ///< @brief 指向状态拥有者
};

} // namespace engine::ui::state
