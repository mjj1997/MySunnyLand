#include "audio_component.h"
#include "../audio/audio_player.h"
#include "../object/game_object.h"
#include "../render/camera.h"
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

void AudioComponent::playSound(std::string_view soundId, bool useSpatial)
{
    // 如果 音效 ID 在映射表中，使用映射表中的路径；否则，直接使用 ID 作为路径
    auto soundPath = m_soundIdToPath.contains(soundId) ? m_soundIdToPath[std::string(soundId)]
                                                       : soundId;

    if (useSpatial && m_transformComponent != nullptr) {
        // TODO: 正式版 SDL3_Mixer 已经支持空间定位，但这里不展开
        // 这里给一个简单的功能：150像素范围内播放，否则不播放
        auto cameraCenter = m_camera->position() + m_camera->viewportSize() / 2.0F;
        auto objectPosition = m_transformComponent->position();
        const float distance{ glm::length(cameraCenter - objectPosition) };
        if (distance > 150.0F) {
            spdlog::debug("AudioComponent:playSound: 音效 '{}' 距离相机太远，不播放。", soundId);
            return;
        }
    }

    m_audioPlayer->playSound(soundPath);
}

void AudioComponent::addSound(std::string_view soundId, std::string_view soundPath)
{
    if (m_soundIdToPath.contains(soundId)) {
        spdlog::warn("AudioComponent:addSound: 音效id '{}' 已存在，覆盖旧路径。", soundId);
    }

    m_soundIdToPath.emplace(soundId, soundPath);
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
