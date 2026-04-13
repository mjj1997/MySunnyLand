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

    m_actionToKeyNames = config->m_inputMappings; // 获取配置中的输入映射（动作 -> 按键名称）
    m_scancodeToActions.clear();
    m_mouseButtonToActions.clear();
    m_actionStates.clear();

    // 如果配置中没有定义鼠标按钮动作(通常不需要配置),则添加默认映射, 用于 UI
    if (m_actionToKeyNames.find("MouseLeftClick") == m_actionToKeyNames.end()) {
        spdlog::debug("配置中没有定义 'MouseLeftClick' 动作,添加默认映射到 'MouseLeft'.");
        m_actionToKeyNames["MouseLeftClick"] = { "MouseLeft" }; // 如果缺失则添加默认映射
    }
    if (m_actionToKeyNames.find("MouseRightClick") == m_actionToKeyNames.end()) {
        spdlog::debug("配置中没有定义 'MouseRightClick' 动作,添加默认映射到 'MouseRight'.");
        m_actionToKeyNames["MouseRightClick"] = { "MouseRight" }; // 如果缺失则添加默认映射
    }

    // 遍历 动作 -> 按键名称 的映射
    for (const auto& [action, keyNames] : m_actionToKeyNames) {
        // 每个动作对应一个动作状态，初始化为 Inactive
        m_actionStates[action] = ActionState::Inactive;

        // 设置 "按键 -> 动作" 的映射
        spdlog::trace("映射动作: {}", action);
        for (const std::string& keyName : keyNames) {
            // 尝试根据按键名称获取scancode
            SDL_Scancode scancode{ scancodeFromString(keyName) };
            // 尝试根据按键名称获取鼠标按钮
            Uint32 mouseButton{ mouseButtonUint32FromString(keyName) };
            // 未来可添加其它输入类型 ...

            if (scancode != SDL_SCANCODE_UNKNOWN) {
                // 如果 scancode 有效,则将 action 添加到 m_scancodeToActions 中的对应列表
                m_scancodeToActions[scancode].push_back(action);
                spdlog::trace("  映射按键: {} (Scancode: {}) 到动作: {}",
                              keyName,
                              static_cast<int>(scancode),
                              action);
            } else if (mouseButton != 0) {
                // 如果鼠标按钮有效,则将 action 添加到 m_mouseButtonToActions 中的对应列表
                m_mouseButtonToActions[mouseButton].push_back(action);
                spdlog::trace("  映射鼠标按钮: {} (Button ID: {}) 到动作: {}",
                              keyName,
                              static_cast<int>(mouseButton),
                              action);
                // else if: 未来可添加其它输入类型 ...
            } else {
                spdlog::warn("输入映射警告: 未知键或按钮名称 '{}' 用于动作 '{}'.", keyName, action);
            }
        }
    }
    spdlog::trace("输入映射初始化完成.");
}

// --- 工具函数 ---

// 将键盘按键名称字符串转换为 SDL_Scancode
SDL_Scancode InputManager::scancodeFromString(const std::string& keyName)
{
    return SDL_GetScancodeFromName(keyName.c_str());
}

// 将鼠标按钮名称字符串转换为 SDL 按钮 Uint32 值
Uint32 InputManager::mouseButtonUint32FromString(const std::string& buttonName)
{
    if (buttonName == "MouseLeft") {
        return SDL_BUTTON_LEFT;
    }
    if (buttonName == "MouseRight") {
        return SDL_BUTTON_RIGHT;
    }
    if (buttonName == "MouseMiddle") {
        return SDL_BUTTON_MIDDLE;
    }
    // 鼠标侧键
    if (buttonName == "MouseX1") {
        return SDL_BUTTON_X1;
    }
    if (buttonName == "MouseX2") {
        return SDL_BUTTON_X2;
    }
    // 0 表示无效按钮
    return 0;
}

} // namespace engine::input
