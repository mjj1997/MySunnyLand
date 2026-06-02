#include "audio_component.h"
#include "../audio/audio_player.h"
#include "../object/game_object.h"
#include "transform_component.h"

#include <spdlog/spdlog.h>

namespace engine::component {

AudioComponent::AudioComponent(engine::audio::AudioPlayer* audioPlayer,
                               engine::render::Camera* camera)
    : m_audioPlayer{ audioPlayer }
    , m_camera{ camera }
{
    if (m_audioPlayer == nullptr || m_camera == nullptr) {
        spdlog::error("AudioComponent 初始化失败，音频播放器或相机为空");
    }
}

void AudioComponent::playSound(const std::string& soundId)
{
    auto soundPath = m_soundIdToPath.find(soundId) != m_soundIdToPath.end()
                         ? m_soundIdToPath[soundId]
                         : soundId;
    m_audioPlayer->playSound(soundPath);
}

void AudioComponent::addSound(const std::string& soundId, const std::string& soundPath)
{
    if (m_soundIdToPath.find(soundId) != m_soundIdToPath.end()) {
        spdlog::warn("AudioComponent:addSound: 音效id '{}' 已存在，覆盖旧路径。", soundId);
    }

    m_soundIdToPath[soundId] = soundPath;
    spdlog::debug("AudioComponent:addSound: 音效id '{}' 已添加，路径为 '{}。", soundId, soundPath);
}

void AudioComponent::init()
{
    if (m_owner == nullptr) {
        spdlog::error("AudioComponent 没有所有者 GameObject!");
        return;
    }

    m_transformComponent = m_owner->getComponent<engine::component::TransformComponent>();
    if (m_transformComponent == nullptr) {
        spdlog::error(
            "AudioComponent 所在的 GameObject 上没有 TransformComponent！无法进行空间定位。");
        return;
    }
}

} // namespace engine::component
