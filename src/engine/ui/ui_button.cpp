#include "ui_button.h"
#include "state/ui_normal_state.h"

#include <spdlog/spdlog.h>

namespace engine::ui {

UiButton::UiButton(engine::core::Context& context,
                   std::string_view normalSpriteId,
                   std::string_view hoverSpriteId,
                   std::string_view pressedSpriteId,
                   glm::vec2 localPosition,
                   glm::vec2 size,
                   std::function<void()> callback)
    : UiInteractiveElementBase{ context, localPosition, size }
    , m_callback{ std::move(callback) }
{
    // 添加各状态对应的精灵
    addSprite("normal", std::make_unique<engine::render::Sprite>(normalSpriteId));
    addSprite("hover", std::make_unique<engine::render::Sprite>(hoverSpriteId));
    addSprite("pressed", std::make_unique<engine::render::Sprite>(pressedSpriteId));

    // 设置默认状态为"normal"
    setCurrentState(std::make_unique<engine::ui::state::UiNormalState>(this));

    // 添加各状态对应的音效
    addSound("hover", "assets/audio/button_hover.wav");
    addSound("pressed", "assets/audio/button_click.wav");

    spdlog::trace("UiButton 构造完成");
}

void UiButton::clicked()
{
    if (m_callback) {
        m_callback();
    }
}

} // namespace engine::ui
