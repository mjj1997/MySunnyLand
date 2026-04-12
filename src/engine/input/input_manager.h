#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace engine::core {
class Configurator;
}

namespace engine::input {

class InputManager final
{
public:
    /**
     * @brief 构造函数
     * @param config 配置对象
     * @throws std::runtime_error 如果任一指针为 nullptr。
     */
    InputManager(const engine::core::Configurator* config);

private:
    ///< @brief 根据 Configurator配置初始化映射表
    void initMappings(const engine::core::Configurator* config);

    ///< @brief 将字符串键名转换为 SDL_Scancode
    SDL_Scancode scancodeFromString(const std::string& keyName);

private:
    ///< @brief 存储动作名称到按键名称列表的映射
    std::unordered_map<std::string, std::vector<std::string>> m_action2KeyNamesMap;
    ///< @brief 从键盘（Scancode）到关联的动作名称列表
    std::unordered_map<SDL_Scancode, std::vector<std::string>> m_scancode2ActionsMap;
    ///< @brief 从鼠标按钮 (Uint32) 到关联的动作名称列表
    std::unordered_map<Uint32, std::vector<std::string>> m_mouseButton2ActionsMap;
};

} // namespace engine::input
