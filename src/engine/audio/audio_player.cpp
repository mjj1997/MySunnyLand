#include "audio_player.h"
#include "../resource/resource_manager.h"

#include <SDL3_mixer/SDL_mixer.h>
#include <spdlog/spdlog.h>

namespace engine::audio {

AudioPlayer::AudioPlayer(engine::resource::ResourceManager* resourceManager)
    : m_resourceManager{ resourceManager }
{
    if (!m_resourceManager) {
        throw std::runtime_error("AudioPlayer 构造失败: 提供的 ResourceManager 指针为空。");
    }

    m_mixer = m_resourceManager->mixer();
    m_musicTrack = MIX_CreateTrack(m_mixer);
    if (!m_musicTrack) {
        throw std::runtime_error("AudioPlayer 构造失败: 无法创建音乐轨道："
                                 + std::string{ SDL_GetError() });
    }
}

AudioPlayer::~AudioPlayer()
{
    if (m_musicTrack) {
        MIX_DestroyTrack(m_musicTrack);
        m_musicTrack = nullptr;
    }
}

int AudioPlayer::playSound(const std::string& soundPath)
{
    MIX_Audio* sound{ m_resourceManager->getSound(soundPath) };
    if (!sound) {
        spdlog::error("AudioPlayer: 无法获取音效 '{}' 播放。", soundPath);
        return -1;
    }

    if (!MIX_PlayAudio(m_mixer, sound)) {
        spdlog::error("AudioPlayer: 无法播放音效 '{}': {}", soundPath, SDL_GetError());
        return -1;
    }

    spdlog::trace("AudioPlayer: 播放音效 '{}'。", soundPath);
    return 0;
}

bool AudioPlayer::playMusic(const std::string& musicPath, int loops, int fadeInTime)
{
    if (musicPath == m_currentMusic) {
        return true; // 如果当前音乐已经在播放，则不重复播放
    }
    m_currentMusic = musicPath;

    MIX_Audio* music{ m_resourceManager->getMusic(musicPath) }; // 通过 ResourceManager 获取资源
    if (!music) {
        spdlog::error("AudioPlayer: 无法获取音乐 '{}' 播放。", musicPath);
        return false;
    }

    MIX_StopTrack(m_musicTrack, 0);         // 立即停止之前的音乐
    MIX_SetTrackAudio(m_musicTrack, music); // 设置音乐轨道的音频源

    // 配置播放参数（循环次数、淡入时长）
    auto props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loops);
    if (fadeInTime > 0) {
        SDL_SetNumberProperty(props, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, fadeInTime);
    }

    // 播放音乐
    bool result{ MIX_PlayTrack(m_musicTrack, props) };
    SDL_DestroyProperties(props);

    if (!result) {
        spdlog::error("AudioPlayer: 无法播放音乐 '{}': {}", musicPath, SDL_GetError());
    } else {
        spdlog::trace("AudioPlayer: 播放音乐 '{}'。", musicPath);
    }

    return result;
}

} // namespace engine::audio
