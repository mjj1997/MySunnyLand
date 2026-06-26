#include "text_renderer.h"
#include "../resource/resource_manager.h"
#include "camera.h"

#include <SDL3_ttf/SDL_ttf.h>
#include <spdlog/spdlog.h>

namespace engine::render {

TextRenderer::TextRenderer(SDL_Renderer* sdlRenderer,
                           engine::resource::ResourceManager* resourceManager)
    : m_sdlRenderer{ sdlRenderer }
    , m_resourceManager{ resourceManager }
{
    if (m_sdlRenderer == nullptr || m_resourceManager == nullptr) {
        throw std::runtime_error("TextRenderer 需要一个有效的 SDL_Renderer 和 ResourceManager。");
    }

    // 初始化 SDL_ttf
    if (TTF_WasInit() == 0 && !TTF_Init()) {
        throw std::runtime_error("初始化 SDL_ttf 失败: " + std::string{ SDL_GetError() });
    }

    m_textEngine = TTF_CreateRendererTextEngine(m_sdlRenderer);
    if (m_textEngine == nullptr) {
        spdlog::error("创建 TTF_TextEngine 失败: {}", SDL_GetError());
        throw std::runtime_error("创建 TTF_TextEngine 失败。");
    }

    spdlog::trace("TextRenderer 初始化成功.");
}

TextRenderer::~TextRenderer()
{
    if (m_textEngine != nullptr) {
        close();
    }
}

void TextRenderer::close()
{
    if (m_textEngine != nullptr) {
        TTF_DestroyRendererTextEngine(m_textEngine);
        m_textEngine = nullptr;
        spdlog::trace("TTF_TextEngine 已销毁。");
    }

    TTF_Quit();
}

void TextRenderer::drawUiText(std::string_view text,
                              std::string_view fontId,
                              int fontSize,
                              const glm::vec2& screenPosition,
                              const engine::utils::FColor& color)
{
    /* 构造函数已经保证了必要指针不会为空，这里不需要再检查 */

    // 获取字体
    TTF_Font* font{ m_resourceManager->getFont(fontId, fontSize) };
    if (font == nullptr) {
        spdlog::warn("drawUiText 获取字体失败: {} 大小 {}", fontId, fontSize);
        return;
    }

    // 创建临时 TTF_Text 对象(目前效率不高，未来可以考虑使用缓存优化)
    TTF_Text* tempTextObject{ TTF_CreateText(m_textEngine, font, text.data(), 0) };
    if (tempTextObject == nullptr) {
        spdlog::error("drawUiText 创建临时 TTF_Text 失败: {}", SDL_GetError());
        return;
    }

    // 先渲染一次黑色文字模拟阴影(偏移 2 像素)
    TTF_SetTextColorFloat(tempTextObject, 0.0F, 0.0F, 0.0F, 1.0F);
    if (!TTF_DrawRendererText(tempTextObject, screenPosition.x + 2, screenPosition.y + 2)) {
        spdlog::error("drawUiText 绘制临时 TTF_Text 失败: {}", SDL_GetError());
    }

    // 然后正常绘制
    TTF_SetTextColorFloat(tempTextObject, color.r, color.g, color.b, color.a);
    if (!TTF_DrawRendererText(tempTextObject, screenPosition.x, screenPosition.y)) {
        spdlog::error("drawUiText 绘制临时 TTF_Text 失败: {}", SDL_GetError());
    }

    // 销毁临时 TTF_Text 对象
    TTF_DestroyText(tempTextObject);
}

void TextRenderer::drawText(const Camera& camera,
                            std::string_view text,
                            std::string_view fontId,
                            int fontSize,
                            const glm::vec2& worldPosition,
                            const engine::utils::FColor& color)
{
    // 应用相机变换
    const glm::vec2 screenPos{ camera.worldToScreen(worldPosition) };

    // 用新坐标调用 drawUiText() 即可
    drawUiText(text, fontId, fontSize, screenPos, color);
}

glm::vec2 TextRenderer::getTextSize(std::string_view text, std::string_view fontId, int fontSize)
{
    /* 构造函数已经保证了必要指针不会为空，这里不需要再检查 */

    // 获取字体
    TTF_Font* font{ m_resourceManager->getFont(fontId, fontSize) };
    if (font == nullptr) {
        spdlog::warn("getTextSize 获取字体失败: {} 大小 {}", fontId, fontSize);
        return glm::vec2(0.0F);
    }

    // 创建临时 TTF_Text 对象
    TTF_Text* tempTextObject{ TTF_CreateText(m_textEngine, font, text.data(), 0) };
    if (tempTextObject == nullptr) {
        spdlog::error("getTextSize 创建临时 TTF_Text 失败: {}", SDL_GetError());
        return glm::vec2(0.0F);
    }

    // 获取文本尺寸
    int width{ 0 };
    int height{ 0 };
    TTF_GetTextSize(tempTextObject, &width, &height);

    // 销毁临时 TTF_Text 对象
    TTF_DestroyText(tempTextObject);

    return glm::vec2{ static_cast<float>(width), static_cast<float>(height) };
}

} // namespace engine::render
