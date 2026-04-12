#include "input_manager.h"
#include "../core/configurator.h"

#include <spdlog/spdlog.h>

#include <stdexcept>

namespace engine::input {

InputManager::InputManager(const engine::core::Configurator* config)
{
    // 初始化映射表
    initMappings(config);
}

// --- 初始化输入映射 ---

void InputManager::initMappings(const engine::core::Configurator* config)
{
    spdlog::trace("初始化输入映射...");
    if (!config) {
        spdlog::error("输入管理器: Config 为空指针");
        throw std::runtime_error("输入管理器: Config 为空指针");
    }

    m_action2KeyNamesMap = config->m_inputMappings; // 获取配置中的输入映射（动作 -> 按键名称）
    m_scancode2ActionsMap.clear();
    m_mouseButton2ActionsMap.clear();

    // 如果配置中没有定义鼠标按钮动作(通常不需要配置),则添加默认映射, 用于 UI
    if (m_action2KeyNamesMap.find("MouseLeftClick") == m_action2KeyNamesMap.end()) {
        spdlog::debug("配置中没有定义 'MouseLeftClick' 动作,添加默认映射到 'MouseLeft'.");
        m_action2KeyNamesMap["MouseLeftClick"] = { "MouseLeft" }; // 如果缺失则添加默认映射
    }
    if (m_action2KeyNamesMap.find("MouseRightClick") == m_action2KeyNamesMap.end()) {
        spdlog::debug("配置中没有定义 'MouseRightClick' 动作,添加默认映射到 'MouseRight'.");
        m_action2KeyNamesMap["MouseRightClick"] = { "MouseRight" }; // 如果缺失则添加默认映射
    }

    // 遍历 动作 -> 按键名称 的映射
    for (const auto& [action, keyNames] : m_action2KeyNamesMap) {
        // 设置 "按键 -> 动作" 的映射
        spdlog::trace("映射动作: {}", action);
        for (const std::string& keyName : keyNames) {
            // TODO: 根据按键名称获取scancode或鼠标按钮
        }
    }
    spdlog::trace("输入映射初始化完成.");
}

} // namespace engine::input
