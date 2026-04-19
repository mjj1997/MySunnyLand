// sprite.h
#pragma once

#include <SDL3/SDL_rect.h> // 用于 SDL_FRect

#include <optional> // 用于 std::optional 表示可选的源矩形
#include <string>

namespace engine::render {

/**
 * @brief 表示要绘制的视觉精灵的数据。
 *
 * 包含纹理标识符、要绘制的纹理部分（源矩形）以及翻转状态。
 * 位置、缩放和旋转由外部（例如 SpriteComponent）标识。
 * 渲染工作由 Renderer 类完成。（传入Sprite作为参数）
 */
class Sprite final
{
public:
    /**
     * @brief 构造一个精灵
     *
     * @param textureId 纹理资源的标识符。不应为空。
     * @param sourceRect 可选的源矩形（SDL_FRect），定义要使用的纹理部分。如果为 std::nullopt，则使用整个纹理。
     * @param isFlipped 是否水平翻转
     */
    explicit Sprite(const std::string& textureId,
                    const std::optional<SDL_FRect>& sourceRect = std::nullopt,
                    bool isFlipped = false)
        : m_textureId(textureId)
        , m_sourceRect(sourceRect)
        , m_isFlipped(isFlipped)
    {}

    // --- getters and setters ---
    const std::string& textureId() const { return m_textureId; } ///< @brief 获取纹理 ID
    ///< @brief 获取源矩形 (如果使用整个纹理则为 std::nullopt)
    const std::optional<SDL_FRect>& sourceRect() const { return m_sourceRect; }
    bool isFlipped() const { return m_isFlipped; } ///< @brief 获取是否水平翻转

    ///< @brief 设置纹理 ID
    void setTextureId(const std::string& textureId) { m_textureId = textureId; }
    ///< @brief 设置源矩形 (如果使用整个纹理则为 std::nullopt)
    void setSourceRect(const std::optional<SDL_FRect>& sourceRect) { m_sourceRect = sourceRect; }
    void setFlipped(bool flipped) { m_isFlipped = flipped; } ///< @brief 设置是否水平翻转

private:
    std::string m_textureId;               ///< @brief 纹理资源的标识符
    std::optional<SDL_FRect> m_sourceRect; ///< @brief 可选：要绘制的纹理部分
    bool m_isFlipped{ false };             ///< @brief 是否水平翻转
};

} // namespace engine::render
