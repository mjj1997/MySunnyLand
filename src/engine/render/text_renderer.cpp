#include "text_renderer.h"

#include <SDL3_ttf/SDL_ttf.h>
#include <spdlog/spdlog.h>

namespace engine::render {

TextRenderer::TextRenderer(SDL_Renderer* sdlRenderer,
                           engine::resource::ResourceManager* resourceManager)
    : m_sdlRenderer{ sdlRenderer }
    , m_resourceManager{ resourceManager }
{
    if (!m_sdlRenderer || !m_resourceManager) {
        throw std::runtime_error("TextRenderer 需要一个有效的 SDL_Renderer 和 ResourceManager。");
    }

    // 初始化 SDL_ttf
    if (!TTF_WasInit() && TTF_Init() == false) {
        throw std::runtime_error("初始化 SDL_ttf 失败: " + std::string{ SDL_GetError() });
    }

    m_textEngine = TTF_CreateRendererTextEngine(m_sdlRenderer);
    if (!m_textEngine) {
        spdlog::error("创建 TTF_TextEngine 失败: {}", SDL_GetError());
        throw std::runtime_error("创建 TTF_TextEngine 失败。");
    }

    spdlog::trace("TextRenderer 初始化成功.");
}

TextRenderer::~TextRenderer()
{
    if (m_textEngine) {
        close();
    }
}

void TextRenderer::close()
{
    if (m_textEngine) {
        TTF_DestroyRendererTextEngine(m_textEngine);
        m_textEngine = nullptr;
        spdlog::trace("TTF_TextEngine 已销毁。");
    }

    TTF_Quit();
}

} // namespace engine::render
