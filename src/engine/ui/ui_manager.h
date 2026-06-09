#pragma once

namespace engine::ui {

/**
 * @brief 管理特定场景中的UI元素集合。
 *
 * 负责 UI 元素的生命周期管理（通过根元素）、渲染调用和输入事件分发。
 * 每个需要 UI 元素的场景（如菜单、游戏HUD）应该拥有一个 UiManager 实例。
 */
class UiManager final
{
public:
    ///< @brief 构造函数将创建默认的根节点。
    UiManager();
    ~UiManager() = default;

    // 禁止拷贝和移动构造/赋值
    UiManager(const UiManager&) = delete;
    UiManager& operator=(const UiManager&) = delete;
    UiManager(UiManager&&) = delete;
    UiManager& operator=(UiManager&&) = delete;
};

} // namespace engine::ui
