#pragma once

#include <SDL3_mixer/SDL_mixer.h> // SDL_mixer 主头文件

#include <memory> // 用于 std::unique_ptr
#include <string>
#include <unordered_map>

namespace engine::resource {

/**
 * @brief 管理 SDL_mixer 音效和音乐(统一为 MIX_Audio 类型)。
 *
 * 提供音频资源的加载和缓存功能。构造失败时会抛出异常。
 * 仅供 ResourceManager 内部使用。
 */
class AudioManager final
{
public:
    /**
     * @brief 构造函数。初始化 SDL_mixer 并创建音频设备混音器。
     * @throws std::runtime_error 如果 SDL_mixer 初始化或创建混音器失败。
     */
    AudioManager();

    ~AudioManager(); ///< @brief 需要手动添加析构函数，清理资源并关闭 SDL_mixer。

    // 当前设计中，我们只需要一个AudioManager，所有权不变，所以不需要拷贝、移动相关构造及赋值运算符
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;
    AudioManager(AudioManager&&) = delete;
    AudioManager& operator=(AudioManager&&) = delete;

    // 仅供 ResourceManager 访问的方法
private:
    MIX_Audio* loadSound(const std::string& filePath); ///< @brief 从文件路径加载音效（预解码）
    ///< @brief 尝试获取已加载音效的指针，如果未加载则尝试加载
    MIX_Audio* getSound(const std::string& filePath);
    void unloadSound(const std::string& filePath); ///< @brief 卸载指定的音效资源
    void clearSounds();                            ///< @brief 清空所有音效资源

    MIX_Audio* loadMusic(const std::string& filePath); ///< @brief 从文件路径加载音乐（流式解码）
    ///< @brief 尝试获取已加载音乐的指针，如果未加载则尝试加载
    MIX_Audio* getMusic(const std::string& filePath);
    void unloadMusic(const std::string& filePath); ///< @brief 卸载指定的音乐资源
    void clearMusics();                            ///< @brief 清空所有音乐资源

    void clearAudio(); ///< @brief 清空所有音频资源

private:
    friend class ResourceManager;

    // MIX_Audio 的自定义删除器
    struct SDLMixAudioDeletor
    {
        void operator()(MIX_Audio* audio) const
        {
            if (audio) {
                MIX_DestroyAudio(audio);
            }
        }
    };

    // 音效存储 (文件路径 -> MIX_Audio, 预解码)
    std::unordered_map<std::string, std::unique_ptr<MIX_Audio, SDLMixAudioDeletor>> m_sounds;
    // 音乐存储 (文件路径 -> MIX_Audio, 流式解码)
    std::unordered_map<std::string, std::unique_ptr<MIX_Audio, SDLMixAudioDeletor>> m_musics;

    MIX_Mixer* m_mixer{ nullptr }; ///< @brief SDL_mixer 混音器实例
};

} // namespace engine::resource
