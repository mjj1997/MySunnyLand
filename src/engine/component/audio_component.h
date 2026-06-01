#pragma once

#include "component_base.h"

#include <string>
#include <unordered_map>

namespace engine::audio {
class AudioPlayer;
}

namespace engine::component {

/**
 * @brief 音频组件，用于处理音频播放和管理。
 */
class AudioComponent final : public ComponentBase
{
    friend class engine::object::GameObject;

public:
    AudioComponent(engine::audio::AudioPlayer* audioPlayer);
    ~AudioComponent() override = default;

    // 禁止拷贝和移动
    AudioComponent(const AudioComponent&) = delete;
    AudioComponent& operator=(const AudioComponent&) = delete;
    AudioComponent(AudioComponent&&) = delete;
    AudioComponent& operator=(AudioComponent&&) = delete;

    /**
     * @brief 播放音效。
     * @param soundId 音效文件的id (或路径)。
     */
    void playSound(const std::string& soundId);

    /**
     * @brief 添加音效到映射表。
     * @param soundId 音效的标识符（针对本组件唯一即可）。
     * @param soundPath 音效文件的路径。
     */
    void addSound(const std::string& soundId, const std::string& soundPath);

protected:
    // 核心循环方法
    void init() override;
    void update(float deltaTime, engine::core::Context& context) override {}

private:
    engine::audio::AudioPlayer* m_audioPlayer; ///< @brief 音频播放器的非拥有指针
    std::unordered_map<std::string, std::string> m_soundIdToPath; ///< @brief 音效id 到路径的映射表
};

} // namespace engine::component
