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
}

AudioPlayer::~AudioPlayer()
{
    if (m_musicTrack) {
        MIX_DestroyTrack(m_musicTrack);
        m_musicTrack = nullptr;
    }
}

} // namespace engine::audio
