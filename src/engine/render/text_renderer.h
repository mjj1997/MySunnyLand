#pragma once

#include "../utils/math.h"

#include <SDL3/SDL_render.h>
#include <glm/vec2.hpp>

#include <string_view>

struct TTF_TextEngine;

namespace engine::resource {
class ResourceManager;
}

namespace engine::render {
class Camera;

/**
 * @brief 使用 SDL_ttf 和 TTF_Text 对象处理文本渲染。
 *
 * 封装 TTF_TextEngine 并提供创建和绘制 TTF_Text 对象的方法，
 * 管理字体加载和颜色设置。
 */
class TextRenderer final
{
public:
    /**
     * @brief 构造 TextRenderer。
     *
     * @param sdlRenderer 有效的 SDL_Renderer 指针。
     * @param resourceManager 有效的 ResourceManager 指针（用于字体加载）。
     * @throws std::runtime_error 如果初始化失败。
     */
    TextRenderer(SDL_Renderer* sdlRenderer, engine::resource::ResourceManager* resourceManager);

    ///< @brief 析构函数，按需调用close()。
    ~TextRenderer();

    // 禁用拷贝和移动语义
    TextRenderer(const TextRenderer&) = delete;
    TextRenderer& operator=(const TextRenderer&) = delete;
    TextRenderer(TextRenderer&&) = delete;
    TextRenderer& operator=(TextRenderer&&) = delete;

    ///< @brief 显式关闭。清理 TTF_TextEngine 并关闭SDL_ttf。
    void close();

    /**
     * @brief 绘制UI上的字符串。
     *        
     * @param text UTF-8 字符串内容。
     * @param fontId 字体 ID。
     * @param fontSize 字体大小。
     * @param screenPosition 屏幕坐标。
     * @param color 文本颜色。(默认为白色)
     */
    void drawUiText(std::string_view text,
                    std::string_view fontId,
                    int fontSize,
                    const glm::vec2& screenPosition,
                    const engine::utils::FColor& color = { 1.0F, 1.0F, 1.0F, 1.0F });

    /**
     * @brief 绘制地图上的字符串。
     *        
     * @param camera 相机
     * @param text UTF-8 字符串内容。
     * @param fontId 字体 ID。
     * @param fontSize 字体大小。
     * @param worldPosition 世界坐标。
     * @param color 文本颜色。(默认为白色)
     */
    void drawText(const Camera& camera,
                  std::string_view text,
                  std::string_view fontId,
                  int fontSize,
                  const glm::vec2& worldPosition,
                  const engine::utils::FColor& color = { 1.0F, 1.0F, 1.0F, 1.0F });

    /**
     * @brief 获取文本的尺寸。
     *
     * @param text 要测量的文本。
     * @param fontId 字体 ID。
     * @param fontSize 字体大小。
     * @return 文本的尺寸。
     */
    glm::vec2 getTextSize(std::string_view text, std::string_view fontId, int fontSize);

private:
    ///< @brief 持有渲染器的非拥有指针
    SDL_Renderer* m_sdlRenderer{ nullptr };
    ///< @brief 持有资源管理器的非拥有指针
    engine::resource::ResourceManager* m_resourceManager{ nullptr };

    TTF_TextEngine* m_textEngine{ nullptr }; ///< @brief 使用SDL3引入的 TTF_TextEngine 来进行绘制
};

} // namespace engine::render
