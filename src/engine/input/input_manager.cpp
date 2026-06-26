#include "input_manager.h"
#include "../core/configurator.h"

#include <SDL3/SDL.h>
#include <glm/vec2.hpp>
#include <spdlog/spdlog.h>

#include <stdexcept>

namespace engine::input {

InputManager::InputManager(SDL_Renderer* sdlRenderer, const engine::core::Configurator* config)
    : m_sdlRenderer(sdlRenderer)
{
    if (m_sdlRenderer == nullptr) {
        spdlog::error("输入管理器: SDL_Renderer 为空指针");
        throw std::runtime_error("输入管理器: SDL_Renderer 为空指针");
    }

    // 初始化映射表
    initMappings(config);

    // 获取初始鼠标位置
    float x{ 0.0F };
    float y{ 0.0F };
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

// --- 状态查询方法 ---

bool InputManager::isActionDown(std::string_view action) const
{
    if (auto iter = m_actionStates.find(action); iter != m_actionStates.end()) {
        return iter->second == ActionState::PressedThisFrame
               || iter->second == ActionState::HeldDown;
    }
    return false;
}

bool InputManager::isActionPressed(std::string_view action) const
{
    if (auto iter = m_actionStates.find(action); iter != m_actionStates.end()) {
        return iter->second == ActionState::PressedThisFrame;
    }
    return false;
}

bool InputManager::isActionReleased(std::string_view action) const
{
    if (auto iter = m_actionStates.find(action); iter != m_actionStates.end()) {
        return iter->second == ActionState::ReleasedThisFrame;
    }
    return false;
}

// --- 初始化输入映射 ---

void InputManager::initMappings(const engine::core::Configurator* config)
{
    spdlog::trace("初始化输入映射...");
    if (config == nullptr) {
        spdlog::error("输入管理器: Config 为空指针");
        throw std::runtime_error("输入管理器: Config 为空指针");
    }

    m_actionToKeyNames = config->m_inputMappings; // 获取配置中的输入映射（动作 -> 按键名称）
    m_inputKeyToActions.clear();
    m_actionStates.clear();

    // 如果配置中没有定义鼠标按钮动作(通常不需要配置),则添加默认映射, 用于 UI
    if (!m_actionToKeyNames.contains("mouseLeftClick")) {
        spdlog::debug("配置中没有定义 'mouseLeftClick' 动作,添加默认映射到 'MouseLeft'.");
        m_actionToKeyNames["mouseLeftClick"] = { "MouseLeft" }; // 如果缺失则添加默认映射
    }
    if (!m_actionToKeyNames.contains("mouseRightClick")) {
        spdlog::debug("配置中没有定义 'mouseRightClick' 动作,添加默认映射到 'MouseRight'.");
        m_actionToKeyNames["mouseRightClick"] = { "MouseRight" }; // 如果缺失则添加默认映射
    }

    // 遍历 动作 -> 按键名称 的映射
    for (const auto& [action, keyNames] : m_actionToKeyNames) {
        // 每个动作对应一个动作状态，初始化为 Inactive
        m_actionStates[action] = ActionState::Inactive;

        // 设置 "按键 -> 动作" 的映射
        spdlog::trace("映射动作: {}", action);
        for (std::string_view keyName : keyNames) {
            // 尝试根据按键名称获取scancode
            SDL_Scancode scancode{ InputManager::scancodeFromString(keyName) };
            // 尝试根据按键名称获取鼠标按钮
            Uint32 mouseButton{ InputManager::mouseButtonUint32FromString(keyName) };
            // 未来可添加其它输入类型 ...

            if (scancode != SDL_SCANCODE_UNKNOWN) {
                // 如果 scancode 有效,则将 action 添加到 m_inputKeyToActions 中的对应列表
                m_inputKeyToActions[scancode].push_back(action);
                spdlog::trace("  映射按键: {} (Scancode: {}) 到动作: {}",
                              keyName,
                              static_cast<int>(scancode),
                              action);
            } else if (mouseButton != 0) {
                // 如果鼠标按钮有效,则将 action 添加到 m_inputKeyToActions 中的对应列表
                m_inputKeyToActions[mouseButton].push_back(action);
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
        // 如果按键有对应的 actions
        if (auto iter = m_inputKeyToActions.find(scancode); iter != m_inputKeyToActions.end()) {
            const std::vector<std::string>& actions{ iter->second };
            const bool isDown{ event.key.down };
            const bool isRepeat{ event.key.repeat };
            for (const std::string_view action : actions) {
                updateActionState(action, isDown, isRepeat); // 更新action状态
            }
        }
        break;
    }
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP: {
        Uint8 button{ event.button.button }; // 获取鼠标按钮
        // 如果鼠标按钮有对应的 actions
        if (auto iter = m_inputKeyToActions.find(button); iter != m_inputKeyToActions.end()) {
            const std::vector<std::string>& actions{ iter->second };
            const bool isDown{ event.button.down };
            for (const std::string_view action : actions) {
                // 鼠标事件不考虑repeat, 所以第三个参数传false
                updateActionState(action, isDown, false); // 更新action状态
            }
        }
        // 在点击时更新鼠标位置
        m_mousePosition = glm::vec2{ event.button.x, event.button.y };
        break;
    }
    case SDL_EVENT_MOUSE_MOTION: // 处理鼠标运动
        m_mousePosition = glm::vec2{ event.motion.x, event.motion.y };
        break;
    case SDL_EVENT_QUIT:
        m_shouldQuit = true;
        break;
    default:
        break;
    }
}

// --- 工具函数 ---

// 将键盘按键名称字符串转换为 SDL_Scancode
SDL_Scancode InputManager::scancodeFromString(std::string_view keyName)
{
    return SDL_GetScancodeFromName(keyName.data());
}

// 将鼠标按钮名称字符串转换为 SDL 按钮 Uint32 值
Uint32 InputManager::mouseButtonUint32FromString(std::string_view buttonName)
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

void InputManager::updateActionState(std::string_view action, bool isInputActive, bool isRepeatEvent)
{
    auto iter = m_actionStates.find(action);
    if (iter == m_actionStates.end()) {
        spdlog::warn("尝试更新未注册动作的状态: {}", action);
        return;
    }

    if (isInputActive) { // 输入被激活（按下）
        if (isRepeatEvent) {
            iter->second = ActionState::HeldDown;
        } else { // 非重复事件
            iter->second = ActionState::PressedThisFrame;
        }
    } else { // 输入被释放（松开）
        iter->second = ActionState::ReleasedThisFrame;
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

glm::vec2 InputManager::logicalMousePosition() const
{
    glm::vec2 logicalPos;
    // 通过窗口坐标获取渲染坐标（逻辑坐标）
    SDL_RenderCoordinatesFromWindow(m_sdlRenderer,
                                    m_mousePosition.x,
                                    m_mousePosition.y,
                                    &logicalPos.x,
                                    &logicalPos.y);
    return logicalPos;
}

} // namespace engine::input
