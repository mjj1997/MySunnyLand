#pragma once

#include "../utils/string_view_hash.h"
#include "component_base.h"

#include <string>
#include <string_view>
#include <unordered_map>

namespace engine::audio {
class AudioPlayer;
}

namespace engine::render {
class Camera;
}

namespace engine::component {
class TransformComponent;

/**
 * @brief 音频组件，用于处理音频播放和管理。
 */
class AudioComponent final : public ComponentBase
{
    friend class engine::object::GameObject;

public:
    AudioComponent(engine::audio::AudioPlayer* audioPlayer, engine::render::Camera* camera);
    ~AudioComponent() override = default;

    // 禁止拷贝和移动
    AudioComponent(const AudioComponent&) = delete;
    AudioComponent& operator=(const AudioComponent&) = delete;
    AudioComponent(AudioComponent&&) = delete;
    AudioComponent& operator=(AudioComponent&&) = delete;

    /**
     * @brief 播放音效。
     * @param soundId 音效文件的id (或路径)。
     * @param useSpatial 是否使用空间定位。
     */
    void playSound(std::string_view soundId, bool useSpatial = false);

    /**
     * @brief 添加音效到映射表。
     * @param soundId 音效的标识符（针对本组件唯一即可）。
     * @param soundPath 音效文件的路径。
     */
    void addSound(std::string_view soundId, std::string_view soundPath);

protected:
    // 核心循环方法
    void init() override;
    void update(float deltaTime, engine::core::Context& context) override {}

private:
    engine::audio::AudioPlayer* m_audioPlayer; ///< @brief 音频播放器的非拥有指针
    engine::render::Camera* m_camera;          ///< @brief 相机的非拥有指针，用于音频空间定位
    engine::component::TransformComponent* m_transformComponent{ nullptr }; ///< @brief 缓存变换组件

    std::unordered_map<std::string, std::string, engine::utils::StringViewHash, std::equal_to<>>
        m_soundIdToPath; ///< @brief 音效id 到路径的映射表
};

} // namespace engine::component
