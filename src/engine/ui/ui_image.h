#pragma once

#include "../render/sprite.h"
#include "ui_element_base.h"

namespace engine::ui {

/**
 * @brief 用于显示纹理或部分纹理的 UI 元素
 *
 * Image 添加了渲染图像的功能。
 */
class UiImage final : public UiElementBase
{
public:
    void render(engine::core::Context& context) override;

    // --- Setters & Getters ---
    const engine::render::Sprite& sprite() const { return m_sprite; }
    void setSprite(const engine::render::Sprite& sprite) { m_sprite = sprite; }

    const std::string& textureId() const { return m_sprite.textureId(); }
    void setTextureId(const std::string& textureId) { m_sprite.setTextureId(textureId); }

    const std::optional<SDL_FRect>& sourceRect() const { return m_sprite.sourceRect(); }
    void setSourceRect(const std::optional<SDL_FRect>& sourceRect)
    {
        m_sprite.setSourceRect(sourceRect);
    }

    bool isFlipped() const { return m_sprite.isFlipped(); }
    void setFlipped(bool flipped) { m_sprite.setFlipped(flipped); }

private:
    engine::render::Sprite m_sprite;
};

} // namespace engine::ui
