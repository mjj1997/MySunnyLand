#include "audio_manager.h"

#include <spdlog/spdlog.h>

#include <stdexcept>

namespace engine::resource {

AudioManager::AudioManager()
{
    // 初始化SDL_mixer
    // SDL_mixer 3.2.0 不再需要传入格式标志
    if (!MIX_Init()) {
        throw std::runtime_error("AudioManager 错误: MIX_Init 失败: "
                                 + std::string{ SDL_GetError() });
    }

    // 创建连接到默认音频输出设备的混音器
    m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (!m_mixer) {
        MIX_Quit();
        throw std::runtime_error("AudioManager 错误: MIX_CreateMixerDevice 失败: "
                                 + std::string{ SDL_GetError() });
    }

    spdlog::trace("AudioManager 构造成功。");
}

AudioManager::~AudioManager()
{
    // 立即停止所有音频播放
    MIX_StopAllTracks(m_mixer, 0);

    // 清理资源映射（unique_ptrs会调用删除器）
    clearAudio();

    // 销毁混音器（同时关闭音频设备）
    MIX_DestroyMixer(m_mixer);

    // 退出SDL_mixer子系统
    MIX_Quit();
    spdlog::trace("AudioManager 析构成功。");
}

// --- 音效管理 ---
MIX_Audio* AudioManager::loadSound(const std::string& filePath)
{
    // 首先检查缓存
    auto it = m_sounds.find(filePath);
    if (it != m_sounds.end()) {
        return it->second.get();
    }

    // 加载音效（predecode=true，预先解码为PCM，适合短音效）
    spdlog::debug("加载音效: {}", filePath);
    MIX_Audio* sound = MIX_LoadAudio(m_mixer, filePath.c_str(), true);
    if (!sound) {
        spdlog::error("加载音效失败: '{}': {}", filePath, SDL_GetError());
        return nullptr;
    }

    // 使用unique_ptr存储在缓存中
    m_sounds.emplace(filePath, std::unique_ptr<MIX_Audio, SDLMixAudioDeletor>(sound));
    spdlog::debug("成功加载并缓存音效: {}", filePath);
    return sound;
}

MIX_Audio* AudioManager::getSound(const std::string& filePath)
{
    auto it = m_sounds.find(filePath);
    if (it != m_sounds.end()) {
        return it->second.get();
    }

    spdlog::warn("音效 '{}' 未找到缓存，尝试加载。", filePath);
    return loadSound(filePath);
}

void AudioManager::unloadSound(const std::string& filePath)
{
    auto it = m_sounds.find(filePath);
    if (it != m_sounds.end()) {
        spdlog::debug("卸载音效: {}", filePath);
        m_sounds.erase(it); // unique_ptr 处理 MIX_DestroyAudio
    } else {
        spdlog::warn("尝试卸载不存在的音效: {}", filePath);
    }
}

void AudioManager::clearSounds()
{
    if (!m_sounds.empty()) {
        spdlog::debug("正在清除所有 {} 个缓存的音效。", m_sounds.size());
        m_sounds.clear(); // unique_ptr 处理删除
    }
}

// --- 音乐管理 ---
MIX_Audio* AudioManager::loadMusic(const std::string& filePath)
{
    // 首先检查缓存
    auto it = m_musics.find(filePath);
    if (it != m_musics.end()) {
        return it->second.get();
    }

    // 加载音乐（predecode=false，流式解码，适合长音乐）
    spdlog::debug("加载音乐: {}", filePath);
    MIX_Audio* music = MIX_LoadAudio(m_mixer, filePath.c_str(), false);
    if (!music) {
        spdlog::error("加载音乐失败: '{}': {}", filePath, SDL_GetError());
        return nullptr;
    }

    // 使用unique_ptr存储在缓存中
    m_musics.emplace(filePath, std::unique_ptr<MIX_Audio, SDLMixAudioDeletor>(music));
    spdlog::debug("成功加载并缓存音乐: {}", filePath);
    return music;
}

MIX_Audio* AudioManager::getMusic(const std::string& filePath)
{
    auto it = m_musics.find(filePath);
    if (it != m_musics.end()) {
        return it->second.get();
    }

    spdlog::warn("音乐 '{}' 未找到缓存，尝试加载。", filePath);
    return loadMusic(filePath);
}

void AudioManager::unloadMusic(const std::string& filePath)
{
    auto it = m_musics.find(filePath);
    if (it != m_musics.end()) {
        spdlog::debug("卸载音乐: {}", filePath);
        m_musics.erase(it); // unique_ptr 处理 MIX_DestroyAudio
    } else {
        spdlog::warn("尝试卸载不存在的音乐: {}", filePath);
    }
}

void AudioManager::clearMusics()
{
    if (!m_musics.empty()) {
        spdlog::debug("正在清除所有 {} 个缓存的音乐曲目。", m_musics.size());
        m_musics.clear(); // unique_ptr 处理删除
    }
}

void AudioManager::clearAudio()
{
    clearSounds();
    clearMusics();
}

} // namespace engine::resource
