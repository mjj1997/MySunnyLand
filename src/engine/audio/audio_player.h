#pragma once

#include <string>

namespace engine::resource {
class ResourceManager;
}

struct MIX_Mixer;
struct MIX_Track;

namespace engine::audio {

/**
 * @brief 用于控制音频播放的类。
 *
 * 提供播放音效和音乐的方法，使用由 ResourceManager 管理的资源。
 * 必须使用有效的 ResourceManager 实例初始化。
 */
class AudioPlayer final
{
public:
    /**
     * @brief 构造函数，使用 ResourceManager 初始化。
     */
    explicit AudioPlayer(engine::resource::ResourceManager* resourceManager);
    ~AudioPlayer();

    // 删除复制/移动操作
    AudioPlayer(const AudioPlayer&) = delete;
    AudioPlayer& operator=(const AudioPlayer&) = delete;
    AudioPlayer(AudioPlayer&&) = delete;
    AudioPlayer& operator=(AudioPlayer&&) = delete;

private:
    ///< @brief 指向 ResourceManager 的非拥有指针，用于加载和管理音频资源。
    engine::resource::ResourceManager* m_resourceManager;
    MIX_Mixer* m_mixer{ nullptr }; ///< @brief 指向 SDL_mixer 混音器的非拥有指针

    MIX_Track* m_musicTrack{ nullptr }; ///< @brief 背景音乐专用播放轨道指针（拥有）
    std::string m_currentMusic; ///< @brief 当前正在播放的音乐路径，用于避免重复播放同一音乐。
};

} // namespace engine::audio
