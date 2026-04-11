#pragma once

#include <nlohmann/json_fwd.hpp> // nlohmann_json 提供的前向声明

#include <string>
#include <unordered_map>
#include <vector>

namespace engine::core {

/**
 * @brief 管理应用程序的配置设置。
 *
 * 提供配置项的默认值，并支持从 JSON 文件加载/保存配置。
 * 如果加载失败或文件不存在，将使用默认值。
 */
class Configurator final
{
public:
    explicit Configurator(const std::string& filePath); ///< @brief 构造函数，指定配置文件路径。

    // 删除拷贝和移动语义
    Configurator(const Configurator&) = delete;
    Configurator& operator=(const Configurator&) = delete;
    Configurator(Configurator&&) = delete;
    Configurator& operator=(Configurator&&) = delete;

    ///< @brief 从指定的 JSON 文件加载配置。成功返回 true，否则返回 false。
    bool loadFromFile(const std::string& filePath);
    ///< @brief 将当前配置保存到指定的 JSON 文件。成功返回 true，否则返回 false。
    [[nodiscard]] bool saveToFile(const std::string& filePath);

public:
    // --- 默认配置值 --- (为了方便拓展，全部设置为公有)
    // 窗口设置
    std::string m_windowTitle{ "SunnyLand" };
    int m_windowWidth{ 1280 };
    int m_windowHeight{ 720 };
    bool m_windowResizable{ true };

    // 图形设置
    bool m_isVSyncEnabled{ true }; ///< @brief 是否启用垂直同步

    // 性能设置
    int m_targetFps{ 144 }; ///< @brief 目标 FPS 设置，0 表示不限制

    // 音频设置
    float m_musicVolume{ 0.5f };
    float m_soundVolume{ 0.5f };

    // 存储动作名称到 SDL Scancode 名称列表的映射
    std::unordered_map<std::string, std::vector<std::string>> m_inputMappings{
        // 提供一些合理的默认值，以防配置文件加载失败或缺少此部分
        { "moveLeft", { "A", "Left" } }, { "moveRight", { "D", "Right" } },
        { "moveUp", { "W", "Up" } },     { "moveDown", { "S", "Down" } },
        { "jump", { "J", "Space" } },    { "attack", { "K", "MouseLeft" } },
        { "pause", { "P", "Escape" } },
        // 可以继续添加更多默认动作
    };

private:
    void fromJson(const nlohmann::json& json); ///< @brief 从 JSON 对象反序列化配置。
    nlohmann::ordered_json toJson() const;     ///< @brief 将当前配置转换为 JSON 对象（按顺序）。
};

} // namespace engine::core
