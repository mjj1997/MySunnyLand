#include "renderer.h"
#include "../resource/resource_manager.h"

#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

#include <stdexcept>

namespace engine::render {

Renderer::Renderer(SDL_Renderer* sdlRenderer, engine::resource::ResourceManager* resourceManager)
    : m_renderer(sdlRenderer)
    , m_resourceManager(resourceManager)
{
    spdlog::trace("构造 Renderer...");

    if (!m_renderer) {
        throw std::runtime_error("Renderer 构造失败: 提供的 SDL_Renderer 指针为空。");
    }
    if (!m_resourceManager) {
        // ResourceManager 是 drawSprite 所必需的
        throw std::runtime_error("Renderer 构造失败: 提供的 ResourceManager 指针为空。");
    }

    setDrawColor(0, 0, 0, 255);

    spdlog::trace("Renderer 构造成功。");
}

void Renderer::present()
{
    SDL_RenderPresent(m_renderer);
}

void Renderer::clearScreen()
{
    if (!SDL_RenderClear(m_renderer)) {
        spdlog::error("清除渲染器失败：{}", SDL_GetError());
    }
}

void Renderer::setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    if (!SDL_SetRenderDrawColor(m_renderer, r, g, b, a)) {
        spdlog::error("设置渲染绘制颜色失败：{}", SDL_GetError());
    }
}

void Renderer::setDrawColorFloat(float r, float g, float b, float a)
{
    if (!SDL_SetRenderDrawColorFloat(m_renderer, r, g, b, a)) {
        spdlog::error("设置渲染绘制颜色失败：{}", SDL_GetError());
    }
}

} // namespace engine::render
