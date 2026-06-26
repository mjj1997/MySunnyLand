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

    if (m_renderer == nullptr) {
        throw std::runtime_error("Renderer 构造失败: 提供的 SDL_Renderer 指针为空。");
    }
    if (m_resourceManager == nullptr) {
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
    if (texture == nullptr) {
        spdlog::error("无法为 ID {} 获取纹理。", sprite.textureId());
        return;
    }

    auto srcRect = getSpriteSrcRect(sprite);
    if (!srcRect.has_value()) {
        spdlog::error("无法获取精灵的源矩形, ID: {}", sprite.textureId());
        return;
    }

    // 应用相机变换
    const glm::vec2 positionScreen{ camera.worldToScreen(position) };

    // 计算目标矩形，注意 position 是精灵的左上角坐标
    const float scaledWidth{ srcRect.value().w * scale.x };
    const float scaledHeight{ srcRect.value().h * scale.y };
    const SDL_FRect destRect{
        .x = positionScreen.x, .y = positionScreen.y, .w = scaledWidth, .h = scaledHeight
    };

    // 视口裁剪：如果精灵超出视口，则不绘制
    if (!Renderer::isRectInViewport(camera, destRect)) {
        // spdlog::info("精灵超出视口范围, ID: {}", sprite.textureId());
        return;
    }

    // 执行绘制(默认旋转中心为精灵的中心点)
    if (!SDL_RenderTextureRotated(m_renderer,
                                  texture,
                                  &srcRect.value(),
                                  &destRect,
                                  angle,
                                  nullptr,
                                  sprite.isFlipped() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE)) {
        spdlog::error("渲染旋转纹理失败(ID: {}): {}", sprite.textureId(), SDL_GetError());
    }
}

void Renderer::drawParallax(const Camera& camera,
                            const Sprite& sprite,
                            const glm::vec2& position,
                            const glm::vec2& scrollFactor,
                            const glm::bvec2& repeat,
                            const glm::vec2& scale)
{
    SDL_Texture* texture{ m_resourceManager->getTexture(sprite.textureId()) };
    if (texture == nullptr) {
        spdlog::error("无法为 ID {} 获取纹理。", sprite.textureId());
        return;
    }

    auto srcRect = getSpriteSrcRect(sprite);
    if (!srcRect.has_value()) {
        spdlog::error("无法获取精灵的源矩形, ID: {}", sprite.textureId());
        return;
    }

    // 应用相机变换
    const glm::vec2 positionScreen{ camera.worldToScreenWithParallax(position, scrollFactor) };

    // 计算缩放后的纹理尺寸
    const float scaledWidth{ srcRect.value().w * scale.x };
    const float scaledHeight{ srcRect.value().h * scale.y };

    glm::vec2 startPos;
    glm::vec2 endPos;
    const glm::vec2 viewportSize{ camera.viewportSize() };
    if (repeat.x) {
        // 重复背景纹理的 x 坐标范围，从 -scaledWidth 到 0
        startPos.x = glm::mod(positionScreen.x, scaledWidth) - scaledWidth;
        endPos.x = viewportSize.x;
    } else {
        startPos.x = positionScreen.x;
        // 结束点是一个纹理宽度之后，但不超过视口宽度
        endPos.x = glm::min(positionScreen.x + scaledWidth, viewportSize.x);
    }
    if (repeat.y) {
        // 重复背景纹理的 y 坐标范围，从 -scaledHeight 到 0
        startPos.y = glm::mod(positionScreen.y, scaledHeight) - scaledHeight;
        endPos.y = viewportSize.y;
    } else {
        startPos.y = positionScreen.y;
        // 结束点是一个纹理高度之后，但不超过视口高度
        endPos.y = glm::min(positionScreen.y + scaledHeight, viewportSize.y);
    }

    for (float posY{ startPos.y }; posY < endPos.y; posY += scaledHeight) {
        for (float posX{ startPos.x }; posX < endPos.x; posX += scaledWidth) {
            const SDL_FRect destRect{ .x = posX, .y = posY, .w = scaledWidth, .h = scaledHeight };
            if (!SDL_RenderTexture(m_renderer, texture, nullptr, &destRect)) {
                spdlog::error("渲染视差纹理失败(ID: {}): {}", sprite.textureId(), SDL_GetError());
                return;
            }
        }
    }
}

void Renderer::drawUiSprite(const Sprite& sprite,
                            const glm::vec2& position,
                            const std::optional<glm::vec2>& size)
{
    SDL_Texture* texture{ m_resourceManager->getTexture(sprite.textureId()) };
    if (texture == nullptr) {
        spdlog::error("无法为 ID {} 获取纹理。", sprite.textureId());
        return;
    }

    auto srcRect = getSpriteSrcRect(sprite);
    if (!srcRect.has_value()) {
        spdlog::error("无法获取精灵的源矩形, ID: {}", sprite.textureId());
        return;
    }

    SDL_FRect destRect{
        .x = position.x, .y = position.y, .w = 0, .h = 0
    }; // 首先确定目标矩形的左上角坐标
    if (size.has_value()) {
        // 如果提供了尺寸，则使用提供的尺寸
        destRect.w = size.value().x;
        destRect.h = size.value().y;
    } else {
        // 如果未提供尺寸，则使用纹理的原始尺寸
        destRect.w = srcRect.value().w;
        destRect.h = srcRect.value().h;
    }

    // 执行绘制(未考虑UI旋转)
    if (!SDL_RenderTextureRotated(m_renderer,
                                  texture,
                                  &srcRect.value(),
                                  &destRect,
                                  0.0,
                                  nullptr,
                                  sprite.isFlipped() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE)) {
        spdlog::error("渲染 UI Sprite 失败 (ID: {}): {}", sprite.textureId(), SDL_GetError());
    }
}

void Renderer::drawUiFilledRect(const engine::utils::Rect& rect, const engine::utils::FColor& color)
{
    setDrawColorFloat(color.r, color.g, color.b, color.a);

    const SDL_FRect sdlRect{
        .x = rect.position.x, .y = rect.position.y, .w = rect.size.x, .h = rect.size.y
    };
    if (!SDL_RenderFillRect(m_renderer, &sdlRect)) {
        spdlog::error("绘制填充矩形失败：{}", SDL_GetError());
    }

    setDrawColor(0, 0, 0, 255);
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

void Renderer::setDrawColor(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha)
{
    if (!SDL_SetRenderDrawColor(m_renderer, red, green, blue, alpha)) {
        spdlog::error("设置渲染绘制颜色失败：{}", SDL_GetError());
    }
}

void Renderer::setDrawColorFloat(float red, float green, float blue, float alpha)
{
    if (!SDL_SetRenderDrawColorFloat(m_renderer, red, green, blue, alpha)) {
        spdlog::error("设置渲染绘制颜色失败：{}", SDL_GetError());
    }
}

std::optional<SDL_FRect> Renderer::getSpriteSrcRect(const Sprite& sprite)
{
    SDL_Texture* texture{ m_resourceManager->getTexture(sprite.textureId()) };
    if (texture == nullptr) {
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
        SDL_FRect defaultRect{ .x = 0.0F, .y = 0.0F, .w = 0.0F, .h = 0.0F };
        if (!SDL_GetTextureSize(texture, &defaultRect.w, &defaultRect.h)) {
            spdlog::error("无法获取纹理尺寸, ID: {}", sprite.textureId());
            return std::nullopt;
        }
        return defaultRect;
    }
}

bool Renderer::isRectInViewport(const Camera& camera, const SDL_FRect& rect)
{
    const glm::vec2 viewportSize{ camera.viewportSize() };
    // 相当于 AABB 碰撞检测
    return rect.x + rect.w >= 0 && rect.x <= viewportSize.x && rect.y + rect.h >= 0
           && rect.y <= viewportSize.y;
}

} // namespace engine::render
