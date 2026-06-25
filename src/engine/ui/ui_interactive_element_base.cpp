#include "ui_interactive_element_base.h"
#include "../audio/audio_player.h"
#include "../core/context.h"
#include "../render/renderer.h"
#include "../resource/resource_manager.h"

#include <spdlog/spdlog.h>

namespace engine::ui {

UiInteractiveElementBase::UiInteractiveElementBase(engine::core::Context& context,
                                                   glm::vec2 localPosition,
                                                   glm::vec2 size)
    : UiElementBase{ localPosition, size }
    , m_context{ context }
{
    spdlog::trace("UIInteractiveElementBase 构造完成");
}

bool UiInteractiveElementBase::handleInput(engine::core::Context& context)
{
    // 先让子 UI 元素处理输入（调用基类的 handleInput 方法）
    if (UiElementBase::handleInput(context)) {
        return true;
    }

    // 子 UI 元素没有处理输入，再自身委托给状态处理输入
    if (m_currentState != nullptr && m_isInteractive) {
        if (auto nextState = m_currentState->handleInput(context); nextState) {
            setCurrentState(std::move(nextState));
            return true;
        }
    }

    return false;
}

void UiInteractiveElementBase::render(engine::core::Context& context)
{
    if (!m_isVisible) {
        return;
    }

    // 先渲染自身
    m_context.renderer().drawUiSprite(*m_currentSprite, screenPosition(), m_size);

    // 再渲染子 UI 元素（调用基类的 render 方法）
    UiElementBase::render(context);
}

void UiInteractiveElementBase::addSprite(std::string_view name,
                                         std::unique_ptr<engine::render::Sprite> sprite)
{
    // 可交互 UI 元素必须有一个 size 用于交互检测，因此如果参数列表中没有指定，则用图片大小作为 size
    if (m_size.x == 0.0F && m_size.y == 0.0F) {
        m_size = m_context.resourceManager().getTextureSize(sprite->textureId());
    }

    // 添加精灵
    m_sprites.emplace(name, std::move(sprite));
}

void UiInteractiveElementBase::addSound(std::string_view name, std::string_view path)
{
    m_sounds.emplace(name, path);
}

void UiInteractiveElementBase::playSound(std::string_view name)
{
    if (auto iter = m_sounds.find(name); iter != m_sounds.end()) {
        m_context.audioPlayer().playSound(iter->second);
    } else {
        spdlog::warn("Sound '{}' 未找到。", name);
    }
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

void UiInteractiveElementBase::setCurrentSprite(std::string_view name)
{
    if (auto iter = m_sprites.find(name); iter != m_sprites.end()) {
        m_currentSprite = iter->second.get();
    } else {
        spdlog::warn("Sprite '{}' 未找到。", name);
    }
}

} // namespace engine::ui
