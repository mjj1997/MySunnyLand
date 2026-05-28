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

} // namespace engine::audio
