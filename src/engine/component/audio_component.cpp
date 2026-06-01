#include "audio_component.h"
#include "../audio/audio_player.h"

#include <spdlog/spdlog.h>

namespace engine::component {

AudioComponent::AudioComponent(engine::audio::AudioPlayer* audioPlayer)
    : m_audioPlayer{ audioPlayer }
{
    if (m_audioPlayer == nullptr) {
        spdlog::error("AudioComponent 初始化失败，音频播放器为空");
    }
}

} // namespace engine::component
