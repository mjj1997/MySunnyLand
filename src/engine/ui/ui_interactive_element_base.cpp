#include "ui_interactive_element_base.h"
#include "../core/context.h"

#include <spdlog/spdlog.h>

namespace engine::ui {

UiInteractiveElementBase::UiInteractiveElementBase(engine::core::Context& context,
                                                   const glm::vec2& localPosition,
                                                   const glm::vec2& size)
    : UiElementBase{ localPosition, size }
    , m_context{ context }
{
    spdlog::trace("UIInteractiveElementBase 构造完成");
}

bool UiInteractiveElementBase::handleInput(engine::core::Context& context)
{
    return false;
}

void UiInteractiveElementBase::render(engine::core::Context& context) {}

void UiInteractiveElementBase::addSprite(const std::string& name,
                                         std::unique_ptr<engine::render::Sprite> sprite)
{
    // 可交互 UI 元素必须有一个 size 用于交互检测，因此如果参数列表中没有指定，则用图片大小作为 size
    if (m_size.x == 0.0f && m_size.y == 0.0f) {
        // TODO: 获取图片大小
    }

    // 添加精灵
    m_sprites[name] = std::move(sprite);
}

void UiInteractiveElementBase::setCurrentState(std::unique_ptr<engine::ui::state::UiStateBase> state)
{
    if (state == nullptr) {
        spdlog::error("尝试设置空的状态！");
        return;
    }

    m_currentState = std::move(state);
    m_currentState->enter();
}

} // namespace engine::ui
