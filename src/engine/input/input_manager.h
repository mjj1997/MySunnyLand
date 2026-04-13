#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace engine::core {
class Configurator;
}

namespace engine::input {

enum class ActionState {
    Inactive,         ///< @brief 动作未激活
    PressedThisFrame, ///< @brief 动作在本帧刚刚被按下
    HeldDown,         ///< @brief 动作被持续按下
    ReleasedThisFrame ///< @brief 动作在本帧刚刚被释放
};

class InputManager final
{
public:
    /**
     * @brief 构造函数
     * @param config 配置对象
     * @throws std::runtime_error 如果任一指针为 nullptr。
     */
    InputManager(const engine::core::Configurator* config);

    void update(); ///< @brief 更新输入状态，每轮循环最先调用

    bool shouldQuit() const;             ///< @brief 查询退出状态
    void setShouldQuit(bool shouldQuit); ///< @brief 设置退出状态

private:
    ///< @brief 根据 Configurator配置初始化映射表
    void initMappings(const engine::core::Configurator* config);
    ///< @brief 处理 SDL 事件（将按键转换为动作状态）
    void processEvent(const SDL_Event& event);

    ///< @brief 将字符串键名转换为 SDL_Scancode
    SDL_Scancode scancodeFromString(const std::string& keyName);
    ///< @brief 将字符串按钮名转换为 SDL_Button
    Uint32 mouseButtonUint32FromString(const std::string& buttonName);
    ///< @brief 辅助更新动作状态
    void updateActionState(const std::string& action, bool isInputActive, bool isRepeatEvent);

private:
    ///< @brief 存储动作名称到按键名称列表的映射
    std::unordered_map<std::string, std::vector<std::string>> m_actionToKeyNames;
    ///< @brief 从键盘（Scancode）到关联的动作名称列表
    std::unordered_map<SDL_Scancode, std::vector<std::string>> m_scancodeToActions;
    ///< @brief 从鼠标按钮 (Uint32) 到关联的动作名称列表
    std::unordered_map<Uint32, std::vector<std::string>> m_mouseButtonToActions;

    std::unordered_map<std::string, ActionState> m_actionStates; ///< @brief 存储每个动作的当前状态

    bool m_shouldQuit{ false }; ///< @brief 退出标志
};

} // namespace engine::input
