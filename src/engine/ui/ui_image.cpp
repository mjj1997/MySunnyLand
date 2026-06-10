#include "ui_image.h"

#include <spdlog/spdlog.h>

namespace engine::ui {

UiImage::UiImage(const std::string& textureId,
                 const glm::vec2& localPosition,
                 const glm::vec2& size,
                 const std::optional<SDL_FRect>& sourceRect,
                 bool isFlipped)
    : UiElementBase{ localPosition, size }
    , m_sprite{ textureId, sourceRect, isFlipped }
{
    if (textureId.empty()) {
        spdlog::warn("创建了一个空纹理 ID 的 UiImage 元素。");
    }

    spdlog::trace("UiImage 构造成功。");
}

void UiImage::render(engine::core::Context& context)
{
}

} // namespace engine::ui
