#include "input_manager.h"
#include "../core/configurator.h"

#include <SDL3/SDL.h>
#include <glm/vec2.hpp>
#include <spdlog/spdlog.h>

#include <stdexcept>

namespace engine::input {

InputManager::InputManager(const engine::core::Configurator* config)
{
    // 初始化映射表
    initMappings(config);

    // 获取初始鼠标位置
    float x;
    float y;
    SDL_GetMouseState(&x, &y);
    m_mousePosition = glm::vec2{ x, y };
    spdlog::trace("初始鼠标位置: ({}, {})", m_mousePosition.x, m_mousePosition.y);
}

// --- 更新和事件处理 ---

void InputManager::update()
{
    // 1. 根据上一帧的值更新默认的动作状态
    for (auto& [action, state] : m_actionStates) {
        if (state == ActionState::PressedThisFrame) {
            state = ActionState::HeldDown; // 当某个键按下不动时，并不会生成SDL_Event。
        } else if (state == ActionState::ReleasedThisFrame) {
            state = ActionState::Inactive;
        }
    }

    // 2. 处理所有待处理的 SDL 事件 (这将设定 m_actionStates 的值)
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        processEvent(event);
    }
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

void InputManager::processEvent(const SDL_Event& event)
{
    switch (event.type) {
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP: {
        SDL_Scancode scancode{ event.key.scancode }; // 获取按键的scancode
        bool isDown{ event.key.down };
        bool isRepeat{ event.key.repeat };

        if (auto it = m_scancodeToActions.find(scancode); it != m_scancodeToActions.end()) {
            // 如果按键有对应的 actions
            const std::vector<std::string>& actions{ it->second };
            for (const std::string& action : actions) {
                updateActionState(action, isDown, isRepeat); // 更新action状态
            }
        }
        break;
    }
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP: {
        Uint8 button{ event.button.button }; // 获取鼠标按钮
        bool isDown{ event.button.down };

        if (auto it = m_mouseButtonToActions.find(button); it != m_mouseButtonToActions.end()) {
            // 如果鼠标按钮有对应的 actions
            const std::vector<std::string>& actions{ it->second };
            for (const std::string& action : actions) {
                // 鼠标事件不考虑repeat, 所以第三个参数传false
                updateActionState(action, isDown, false); // 更新action状态
            }
        }
        break;
    }
    case SDL_EVENT_QUIT:
        m_shouldQuit = true;
        break;
    default:
        break;
    }
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

void InputManager::updateActionState(const std::string& action,
                                     bool isInputActive,
                                     bool isRepeatEvent)
{
    auto it = m_actionStates.find(action);
    if (it == m_actionStates.end()) {
        spdlog::warn("尝试更新未注册动作的状态: {}", action);
        return;
    }

    if (isInputActive) { // 输入被激活（按下）
        if (isRepeatEvent) {
            it->second = ActionState::HeldDown;
        } else { // 非重复事件
            it->second = ActionState::PressedThisFrame;
        }
    } else { // 输入被释放（松开）
        it->second = ActionState::ReleasedThisFrame;
    }
}

bool InputManager::shouldQuit() const
{
    return m_shouldQuit;
}

void InputManager::setShouldQuit(bool shouldQuit)
{
    m_shouldQuit = shouldQuit;
}

glm::vec2 InputManager::mousePosition() const
{
    return m_mousePosition;
}

} // namespace engine::input
