#include "texture_manager.h"

#include <SDL3_image/SDL_image.h> // 用于 IMG_LoadTexture， IMG_Init, IMG_Quit
#include <spdlog/spdlog.h>
#include <stdexcept> // 用于 std::runtime_error

namespace engine::resource {

TextureManager::TextureManager(SDL_Renderer* renderer)
    : m_renderer{ renderer }
{
    if (!renderer) {
        // 关键错误，无法继续，抛出异常 （它将由catch语句捕获（位于GameApp），并进行处理）
        throw std::runtime_error("TextureManager 构造失败：渲染器指针为空。");
    }
    // SDL3中不再需要手动调用IMG_Init/IMG_Quit
    spdlog::trace("TextureManager 构造成功。");
}

SDL_Texture* TextureManager::loadTexture(const std::string& filePath)
{
    // 检查是否已加载该纹理
    auto it = m_textures.find(filePath);
    if (it != m_textures.end()) {
        return it->second.get();
    }

    // 如果未加载，则尝试加载纹理
    SDL_Texture* texture = IMG_LoadTexture(m_renderer, filePath.c_str());
    if (texture == nullptr) {
        spdlog::error("加载纹理失败：'{}': {}", filePath, SDL_GetError());
        return nullptr;
    }

    // 使用带有自定义删除器的 unique_ptr 存储加载的纹理
    m_textures.emplace(filePath, std::unique_ptr<SDL_Texture, SDLTextureDeletor>(texture));
    spdlog::debug("成功加载并缓存纹理：{}", filePath);

    return texture;
}

SDL_Texture* TextureManager::getTexture(const std::string& filePath)
{
    // 查找现有纹理
    auto it = m_textures.find(filePath);
    if (it != m_textures.end()) {
        return it->second.get();
    }

    // 如果未找到纹理，尝试加载纹理
    spdlog::warn("纹理 '{}' 未找到缓存，尝试加载。", filePath);
    return loadTexture(filePath);
}

glm::vec2 TextureManager::getTextureSize(const std::string& filePath)
{
    // 获取纹理
    SDL_Texture* texture{ getTexture(filePath) };
    if (!texture) {
        spdlog::error("无法获取纹理：{}", filePath);
        return glm::vec2{ 0 };
    }

    // 获取纹理尺寸
    glm::vec2 size;
    if (!SDL_GetTextureSize(texture, &size.x, &size.y)) {
        spdlog::error("无法查询纹理尺寸：{}", filePath);
        return glm::vec2{ 0 };
    }
    return size;
}

void TextureManager::unloadTexture(const std::string& filePath)
{
    auto it = m_textures.find(filePath);
    if (it != m_textures.end()) {
        m_textures.erase(it); // unique_ptr 通过自定义删除器自动释放纹理
        spdlog::debug("成功卸载纹理：{}", filePath);
    } else {
        spdlog::warn("尝试卸载不存在的纹理：{}", filePath);
    }
}

void TextureManager::clearTextures()
{
    if (!m_textures.empty()) {
        spdlog::debug("正在清楚所有 {} 个缓存的纹理。", m_textures.size());
        m_textures.clear();
    }
}

} // namespace engine::resource
