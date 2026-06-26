#include "audio_player.h"
#include "../resource/resource_manager.h"

#include <SDL3_mixer/SDL_mixer.h>
#include <spdlog/spdlog.h>

namespace engine::audio {

AudioPlayer::AudioPlayer(engine::resource::ResourceManager* resourceManager)
    : m_resourceManager{ resourceManager }
{
    if (m_resourceManager == nullptr) {
        throw std::runtime_error("AudioPlayer 构造失败: 提供的 ResourceManager 指针为空。");
    }

    m_mixer = m_resourceManager->mixer();
    m_musicTrack = MIX_CreateTrack(m_mixer);
    if (m_musicTrack == nullptr) {
        throw std::runtime_error("AudioPlayer 构造失败: 无法创建音乐轨道："
                                 + std::string(SDL_GetError()));
    }
}

AudioPlayer::~AudioPlayer()
{
    if (m_musicTrack != nullptr) {
        MIX_DestroyTrack(m_musicTrack);
        m_musicTrack = nullptr;
    }
}

int AudioPlayer::playSound(std::string_view soundPath)
{
    MIX_Audio* sound{ m_resourceManager->getSound(soundPath) };
    if (sound == nullptr) {
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

bool AudioPlayer::playMusic(std::string_view musicPath, int loops, int fadeInTime)
{
    if (musicPath == m_currentMusic) {
        return true; // 如果当前音乐已经在播放，则不重复播放
    }
    m_currentMusic = musicPath;

    MIX_Audio* music{ m_resourceManager->getMusic(musicPath) }; // 通过 ResourceManager 获取资源
    if (music == nullptr) {
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
    const bool result{ MIX_PlayTrack(m_musicTrack, props) };
    SDL_DestroyProperties(props);

    if (!result) {
        spdlog::error("AudioPlayer: 无法播放音乐 '{}': {}", musicPath, SDL_GetError());
    } else {
        spdlog::trace("AudioPlayer: 播放音乐 '{}'。", musicPath);
    }

    return result;
}

void AudioPlayer::stopMusic(int fadeOutTime)
{
    const Sint64 fadeFrames{ (fadeOutTime > 0) ? MIX_TrackMSToFrames(m_musicTrack, fadeOutTime)
                                               : 0 };
    MIX_StopTrack(m_musicTrack, fadeFrames);
    m_currentMusic.clear();
    spdlog::trace("AudioPlayer: 停止音乐。");
}

void AudioPlayer::pauseMusic()
{
    MIX_PauseTrack(m_musicTrack);
    spdlog::trace("AudioPlayer: 暂停音乐。");
}

void AudioPlayer::resumeMusic()
{
    MIX_ResumeTrack(m_musicTrack);
    spdlog::trace("AudioPlayer: 恢复音乐。");
}

void AudioPlayer::setSoundVolume(float volume)
{
    // 通过混音器整体的增益控制音效音量（0.0-1.0）
    MIX_SetMixerGain(m_mixer, volume);
    spdlog::trace("AudioPlayer: 设置音效音量为 {:.2f}。", volume);
}

void AudioPlayer::setMusicVolume(float volume)
{
    // 通过音乐轨道的增益控制音乐音量（0.0-1.0）
    MIX_SetTrackGain(m_musicTrack, volume);
    spdlog::trace("AudioPlayer: 设置音乐音量为 {:.2f}。", volume);
}

float AudioPlayer::musicVolume() const
{
    return MIX_GetTrackGain(m_musicTrack);
}

float AudioPlayer::soundVolume() const
{
    return MIX_GetMixerGain(m_mixer);
}

} // namespace engine::audio
