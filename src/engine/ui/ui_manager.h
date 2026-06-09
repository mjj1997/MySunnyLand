#pragma once

#include <memory>

namespace engine::ui {
class UiPanel; // UiPanel 将作为根元素
} // namespace engine::ui

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

    // --- getter ---
    ///< @brief 获取根 UiPanel 元素的指针。
    UiPanel* rootElement() const { return m_rootElement.get(); }

private:
    ///< @brief 一个 UiPanel 作为根节点(UI元素)
    std::unique_ptr<UiPanel> m_rootElement;
};

} // namespace engine::ui
