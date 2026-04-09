#pragma once

#include <glm/glm.hpp>

#include <optional>

struct SDL_Renderer;
struct SDL_FRect;

namespace engine::resource {
class ResourceManager;
}

namespace engine::render {

class Sprite;

/**
 * @brief 封装 SDL3 渲染操作
 *
 * 包装 SDL_Renderer 并提供清除屏幕、绘制精灵和呈现最终图像的方法。
 * 在构造时初始化。依赖于一个有效的 SDL_Renderer 和 ResourceManager。
 * 构造失败会抛出异常。
 */
class Renderer final
{
public:
    /**
     * @brief 构造函数
     *
     * @param sdlRenderer 指向有效的 SDL_Renderer 的指针。不能为空。
     * @param resourceManager 指向有效的 ResourceManager 的指针。不能为空。
     * @throws std::runtime_error 如果任一指针为 nullptr。
     */
    Renderer(SDL_Renderer* sdlRenderer, engine::resource::ResourceManager* resourceManager);

    // 禁用拷贝和移动语义
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    // --- 封装 SDL 渲染函数 ---
    void present();     ///< @brief 更新屏幕，包装 SDL_RenderPresent 函数
    void clearScreen(); ///< @brief 清屏，包装 SDL_RenderClear 函数

    ///< @brief 设置绘制颜色，包装 SDL_SetRenderDrawColor 函数，使用 Uint8 类型
    void setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
    ///< @brief 设置绘制颜色，包装 SDL_SetRenderDrawColorFloat 函数，使用 float 类型
    void setDrawColorFloat(float r, float g, float b, float a = 1.0f);

    // --- getters and setters ---
    ///< @brief 获取底层的 SDL_Renderer 指针
    SDL_Renderer* renderer() const { return m_renderer; }

private:
    ///< @brief 获取精灵的源矩形，用于具体绘制。出现错误则返回std::nullopt并跳过绘制
    std::optional<SDL_FRect> getSpriteSrcRect(const Sprite& sprite);

private:
    ///< @brief 指向 SDL_Renderer 的非拥有指针
    SDL_Renderer* m_renderer{ nullptr };
    ///< @brief 指向 ResourceManager 的非拥有指针
    engine::resource::ResourceManager* m_resourceManager{ nullptr };
};

} // namespace engine::render
