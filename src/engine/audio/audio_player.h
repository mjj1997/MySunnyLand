#pragma once

#include <string>
#include <string_view>

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

    // --- 播放控制方法 ---
    /**
     * @brief 播放音效。
     * 如果尚未缓存，则通过 ResourceManager 加载音效。
     * @param soundPath 音效文件的路径。
     * @return 成功时返回 0，出错时返回 -1。
     */
    int playSound(std::string_view soundPath);

    /**
     * @brief 播放背景音乐。如果正在播放，则淡出之前的音乐。
     * 如果尚未缓存，则通过 ResourceManager 加载音乐。
     * @param musicPath 音乐文件的路径。
     * @param loops 循环次数（-1 无限循环，0 播放一次，1 播放两次，以此类推）。默认为 -1。
     * @param fadeInTime 音乐淡入的时间（毫秒）（0 表示不淡入）。默认为 0。
     * @return 成功返回 true，出错返回 false。
     */
    bool playMusic(std::string_view musicPath, int loops = -1, int fadeInTime = 0);

    /**
     * @brief 停止当前正在播放的背景音乐。
     * @param fadeOutTime 淡出时间（毫秒）（0 表示立即停止）。默认为 0。
     */
    void stopMusic(int fadeOutTime = 0);

    /**
     * @brief 暂停当前正在播放的背景音乐。
     */
    void pauseMusic();

    /**
     * @brief 恢复已暂停的背景音乐。
     */
    void resumeMusic();

    /**
     * @brief 设置音效通道的音量。
     * @param volume 音量级别（0.0-1.0）。
     */
    void setSoundVolume(float volume);

    /**
     * @brief 设置音乐通道的音量。
     * @param volume 音量级别（0.0-1.0）。
     */
    void setMusicVolume(float volume);

    /**
     * @brief 获取当前音乐音量。
     * @return 音量级别（0.0-1.0）。
     */
    float musicVolume() const;

    /**
     * @brief 获取当前音效音量。
     * @return 音量级别（0.0-1.0）。
     */
    float soundVolume() const;

private:
    ///< @brief 指向 ResourceManager 的非拥有指针，用于加载和管理音频资源。
    engine::resource::ResourceManager* m_resourceManager;
    MIX_Mixer* m_mixer{ nullptr }; ///< @brief 指向 SDL_mixer 混音器的非拥有指针

    MIX_Track* m_musicTrack{ nullptr }; ///< @brief 背景音乐专用播放轨道指针（拥有）
    std::string m_currentMusic; ///< @brief 当前正在播放的音乐路径，用于避免重复播放同一音乐。
};

} // namespace engine::audio
