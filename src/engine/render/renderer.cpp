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

    spdlog::trace("Renderer 构造成功。");
}

} // namespace engine::render
