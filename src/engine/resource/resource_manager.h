#pragma once

#include <glm/glm.hpp>

#include <memory> // 用于 std::unique_ptr
#include <string>
#include <string_view>

// 前向声明 SDL 类型
struct SDL_Renderer;
struct SDL_Texture;
struct MIX_Audio;
struct MIX_Mixer;
struct TTF_Font;

namespace engine::resource {

// 前向声明内部管理器
class TextureManager;
class AudioManager;
class FontManager;

/**
 * @brief 作为访问各种资源管理器的中央控制点（外观模式 Facade）。
 * 在构造时初始化其管理的子系统。构造失败会抛出异常。
 */
class ResourceManager final
{
public:
    /**
     * @brief 构造函数，执行初始化。
     * @param sdlRenderer SDL_Renderer 的指针，传递给需要它的子管理器。不能为空。
     */
    explicit ResourceManager(SDL_Renderer* sdlRenderer);

    ~ResourceManager(); // 显式声明析构函数，这是为了能让智能指针正确管理仅有前向声明的类

    ///< @brief 清空所有资源
    void clear();

    // 当前设计中，我们只需要一个ResourceManager，所有权不变，所以不需要拷贝、移动相关构造及赋值运算符
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    // --- 统一资源访问接口 ---
    // -- Texture --
    SDL_Texture* loadTexture(std::string_view filePath); ///< @brief 载入纹理资源
    ///< @brief 尝试获取已加载纹理的指针，如果未加载则尝试加载
    SDL_Texture* getTexture(std::string_view filePath);
    void unloadTexture(std::string_view filePath);       ///< @brief 卸载指定的纹理资源
    glm::vec2 getTextureSize(std::string_view filePath); ///< @brief 获取指定纹理的尺寸
    void clearTextures();                                ///< @brief 清空所有纹理资源

    // -- Sound Effects --
    MIX_Audio* loadSound(std::string_view filePath); ///< @brief 载入音效资源
    ///< @brief 尝试获取已加载音效的指针，如果未加载则尝试加载
    MIX_Audio* getSound(std::string_view filePath);
    void unloadSound(std::string_view filePath); ///< @brief 卸载指定的音效资源
    void clearSounds();                          ///< @brief 清空所有音效资源

    // -- Music --
    MIX_Audio* loadMusic(std::string_view filePath); ///< @brief 载入音乐资源
    ///< @brief 尝试获取已加载音乐的指针，如果未加载则尝试加载
    MIX_Audio* getMusic(std::string_view filePath);
    void unloadMusic(std::string_view filePath); ///< @brief 卸载指定的音乐资源
    void clearMusics();                          ///< @brief 清空所有音乐资源
    // - Mixer -
    MIX_Mixer* mixer() const; ///< @brief 获取 SDL_mixer 混音器指针

    // -- Fonts --
    TTF_Font* loadFont(std::string_view filePath, int pointSize); ///< @brief 载入字体资源
    ///< @brief 尝试获取已加载字体的指针，如果未加载则尝试加载
    TTF_Font* getFont(std::string_view filePath, int pointSize);
    void unloadFont(std::string_view filePath, int pointSize); ///< @brief 卸载指定的字体资源
    void clearFonts();                                         ///< @brief 清空所有字体资源

private:
    // 使用 unique_ptr 确保所有权和自动清理
    std::unique_ptr<TextureManager> m_textureManager;
    std::unique_ptr<AudioManager> m_audioManager;
    std::unique_ptr<FontManager> m_fontManager;
};

} // namespace engine::resource
