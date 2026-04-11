#include "configurator.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <fstream>

namespace engine::core {

Configurator::Configurator(const std::string& filePath)
{
    loadFromFile(filePath);
}

bool Configurator::loadFromFile(const std::string& filePath)
{
    std::ifstream file{ filePath };
    if (!file.is_open()) {
        // 配置文件不存在，使用默认设置并创建默认配置文件
        spdlog::warn("配置文件 '{}' 未找到。使用默认设置并创建默认配置文件。", filePath);
        if (!saveToFile(filePath)) {
            spdlog::error("无法创建默认配置文件 '{}'。", filePath);
            return false;
        }
        return false;
    }

    try {
        nlohmann::json json;
        file >> json;
        fromJson(json);
        spdlog::info("成功从配置文件 '{}' 加载配置。", filePath);
        return true;
    } catch (const std::exception& e) {
        spdlog::error("从配置文件 '{}' 加载配置时出错：{}。使用默认配置。", filePath, e.what());
    }
    return false;
}

bool Configurator::saveToFile(const std::string& filePath)
{
    std::ofstream file{ filePath };
    if (!file.is_open()) {
        spdlog::error("无法打开配置文件 '{}'。", filePath);
        return false;
    }

    try {
        nlohmann::ordered_json json{ toJson() };
        file << json.dump(4);
        spdlog::info("成功将配置保存到文件 '{}'。", filePath);
        return true;
    } catch (const std::exception& e) {
        spdlog::error("保存配置到文件 '{}' 时出错：{}。", filePath, e.what());
    }
    return false;
}

void Configurator::fromJson(const nlohmann::json& json)
{
    if (json.contains("window")) {
        const auto& windowConfig = json["window"];
        m_windowTitle = windowConfig.value("title", m_windowTitle);
        m_windowWidth = windowConfig.value("width", m_windowWidth);
        m_windowHeight = windowConfig.value("height", m_windowHeight);
        m_windowResizable = windowConfig.value("resizable", m_windowResizable);
    }

    if (json.contains("graphics")) {
        const auto& graphicsConfig = json["graphics"];
        m_isVSyncEnabled = graphicsConfig.value("VSync", m_isVSyncEnabled);
    }

    if (json.contains("performance")) {
        const auto& performanceConfig = json["performance"];
        m_targetFps = performanceConfig.value("targetFps", m_targetFps);
        if (m_targetFps < 0) {
            spdlog::warn("目标 FPS 不能为负数。设置为 0（无限制）。");
            m_targetFps = 0;
        }
    }

    if (json.contains("audio")) {
        const auto& audioConfig = json["audio"];
        m_musicVolume = audioConfig.value("musicVolume", m_musicVolume);
        m_soundVolume = audioConfig.value("soundVolume", m_soundVolume);
    }

    if (json.contains("inputMappings") && json["inputMappings"].is_object()) {
        const auto& inputMappingsConfig = json["inputMappings"];
        try {
            // 直接尝试从 JSON 对象转换为 unordered_map<string, vector<string>>
            auto inputMappings
                = inputMappingsConfig
                      .get<std::unordered_map<std::string, std::vector<std::string>>>();
            // 如果成功，则将 inputMappings 移动到 m_inputMappings
            m_inputMappings = std::move(inputMappings);
            spdlog::trace("成功从 JSON 配置加载输入映射。");
        } catch (const std::exception& e) {
            spdlog::warn("配置加载警告：解析 'inputMappings' 时发生异常。使用默认映射。错误：{}",
                         e.what());
        }
    } else {
        spdlog::trace(
            "配置跟踪：未找到 'inputMappings' 部分或不是对象。使用头文件中定义的默认映射。");
    }
}

nlohmann::ordered_json Configurator::toJson() const
{
    return nlohmann::ordered_json{
        { "window",
          { { "title", m_windowTitle },
            { "width", m_windowWidth },
            { "height", m_windowHeight },
            { "resizable", m_windowResizable } } },
        { "graphics", { { "VSync", m_isVSyncEnabled } } },
        { "performance", { { "targetFps", m_targetFps } } },
        { "audio", { { "musicVolume", m_musicVolume }, { "soundVolume", m_soundVolume } } },
        { "inputMappings", m_inputMappings }
    };
}

} // namespace engine::core
