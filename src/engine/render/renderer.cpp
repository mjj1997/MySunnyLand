#include "renderer.h"
#include "../resource/resource_manager.h"
#include "camera.h"
#include "sprite.h"

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

void Renderer::drawSprite(const Camera& camera,
                          const Sprite& sprite,
                          const glm::vec2& position,
                          const glm::vec2& scale,
                          double angle)
{
    SDL_Texture* texture{ m_resourceManager->getTexture(sprite.textureId()) };
    if (!texture) {
        spdlog::error("无法为 ID {} 获取纹理。", sprite.textureId());
        return;
    }

    auto srcRect = getSpriteSrcRect(sprite);
    if (!srcRect.has_value()) {
        spdlog::error("无法获取精灵的源矩形, ID: {}", sprite.textureId());
        return;
    }

    // 应用相机变换
    glm::vec2 positionScreen{ camera.worldToScreen(position) };

    // 计算目标矩形，注意 position 是精灵的左上角坐标
    float scaledWidth{ srcRect.value().w * scale.x };
    float scaledHeight{ srcRect.value().h * scale.y };
    SDL_FRect destRect{ positionScreen.x, positionScreen.y, scaledWidth, scaledHeight };

    // 视口裁剪：如果精灵超出视口，则不绘制
    if (!isRectInViewport(camera, destRect)) {
        spdlog::info("精灵超出视口范围, ID: {}", sprite.textureId());
        return;
    }

    // 执行绘制(默认旋转中心为精灵的中心点)
    if (!SDL_RenderTextureRotated(m_renderer,
                                  texture,
                                  &srcRect.value(),
                                  &destRect,
                                  angle,
                                  NULL,
                                  sprite.isFlipped() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE)) {
        spdlog::error("渲染旋转纹理失败(ID: {}): {}", sprite.textureId(), SDL_GetError());
    }
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

std::optional<SDL_FRect> Renderer::getSpriteSrcRect(const Sprite& sprite)
{
    SDL_Texture* texture{ m_resourceManager->getTexture(sprite.textureId()) };
    if (!texture) {
        spdlog::error("无法为 ID {} 获取纹理。", sprite.textureId());
        return std::nullopt;
    }

    auto srcRect = sprite.sourceRect();
    if (srcRect.has_value()) {
        if (srcRect.value().w <= 0 || srcRect.value().h <= 0) {
            spdlog::error("源矩形尺寸无效, ID: {}", sprite.textureId());
            return std::nullopt;
        }
        return srcRect;
    } else {
        SDL_FRect defaultRect{ 0, 0, 0, 0 };
        if (!SDL_GetTextureSize(texture, &defaultRect.w, &defaultRect.h)) {
            spdlog::error("无法获取纹理尺寸, ID: {}", sprite.textureId());
            return std::nullopt;
        }
        return defaultRect;
    }
}

bool Renderer::isRectInViewport(const Camera& camera, const SDL_FRect& rect)
{
    glm::vec2 viewportSize{ camera.viewportSize() };
    // 相当于 AABB 碰撞检测
    return rect.x + rect.w >= 0 && rect.x <= viewportSize.x && rect.y + rect.h >= 0
           && rect.y <= viewportSize.y;
}

} // namespace engine::render
