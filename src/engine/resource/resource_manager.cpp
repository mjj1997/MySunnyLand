#include "resource_manager.h"
#include "audio_manager.h"
#include "font_manager.h"
#include "texture_manager.h"

#include <spdlog/spdlog.h>

namespace engine::resource {

ResourceManager::ResourceManager(SDL_Renderer* renderer)
{
    // --- 初始化各个子系统 --- (如果出现错误会抛出异常，由上层捕获)
    m_textureManager = std::make_unique<TextureManager>(renderer);
    m_audioManager = std::make_unique<AudioManager>();
    m_fontManager = std::make_unique<FontManager>();

    spdlog::trace("ResourceManager 构造成功。");
}

ResourceManager::~ResourceManager() = default;

void ResourceManager::clear()
{
    m_textureManager->clearTextures();
    m_audioManager->clearAudio();
    m_fontManager->clearFonts();
    spdlog::trace("ResourceManager 中的资源通过 clear() 清空。");
}

// --- 纹理接口实现 ---
SDL_Texture* ResourceManager::loadTexture(const std::string& filePath)
{
    return m_textureManager->loadTexture(filePath);
}

SDL_Texture* ResourceManager::getTexture(const std::string& filePath)
{
    return m_textureManager->getTexture(filePath);
}

void ResourceManager::unloadTexture(const std::string& filePath)
{
    m_textureManager->unloadTexture(filePath);
}

glm::vec2 ResourceManager::getTextureSize(const std::string& filePath)
{
    return m_textureManager->getTextureSize(filePath);
}

void ResourceManager::clearTextures()
{
    m_textureManager->clearTextures();
}

// --- 音频接口实现 ---
MIX_Audio* ResourceManager::loadSound(const std::string& filePath)
{
    return m_audioManager->loadSound(filePath);
}

MIX_Audio* ResourceManager::getSound(const std::string& filePath)
{
    return m_audioManager->getSound(filePath);
}

void ResourceManager::unloadSound(const std::string& filePath)
{
    m_audioManager->unloadSound(filePath);
}

void ResourceManager::clearSounds()
{
    m_audioManager->clearSounds();
}

MIX_Audio* ResourceManager::loadMusic(const std::string& filePath)
{
    return m_audioManager->loadMusic(filePath);
}

MIX_Audio* ResourceManager::getMusic(const std::string& filePath)
{
    return m_audioManager->getMusic(filePath);
}

void ResourceManager::unloadMusic(const std::string& filePath)
{
    m_audioManager->unloadMusic(filePath);
}

void ResourceManager::clearMusics()
{
    m_audioManager->clearMusics();
}

// --- 字体接口实现 ---
TTF_Font* ResourceManager::loadFont(const std::string& filePath, int pointSize)
{
    return m_fontManager->loadFont(filePath, pointSize);
}

TTF_Font* ResourceManager::getFont(const std::string& filePath, int pointSize)
{
    return m_fontManager->getFont(filePath, pointSize);
}

void ResourceManager::unloadFont(const std::string& filePath, int pointSize)
{
    m_fontManager->unloadFont(filePath, pointSize);
}

void ResourceManager::clearFonts()
{
    m_fontManager->clearFonts();
}

} // namespace engine::resource
