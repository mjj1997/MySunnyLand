#pragma once

#include <glm/vec2.hpp>

#include <memory>

namespace engine::core {
class Context;
}

namespace engine::ui {
class UiElementBase;
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
    ~UiManager();

    // 禁止拷贝和移动构造/赋值
    UiManager(const UiManager&) = delete;
    UiManager& operator=(const UiManager&) = delete;
    UiManager(UiManager&&) = delete;
    UiManager& operator=(UiManager&&) = delete;

    ///< @brief 初始化 UI 管理器，设置根元素的大小。
    [[nodiscard]] bool init(const glm::vec2& windowSize);

    // --- 核心循环方法 ---
    ///< @brief 处理输入事件，如果事件被处理则返回true。
    bool handleInput(engine::core::Context& context);
    void update(float deltaTime, engine::core::Context& context);
    void render(engine::core::Context& context);

    // --- element methods ---
    ///< @brief 添加一个UI元素到根节点的 m_children 容器中。
    void addElement(std::unique_ptr<UiElementBase> element);
    void clearElements(); ///< @brief 清除所有UI元素，通常用于重置UI状态。

    // --- getter ---
    ///< @brief 获取根 UiPanel 元素的指针。
    UiPanel* rootElement() const { return m_rootElement.get(); }

private:
    ///< @brief 一个 UiPanel 作为根节点(UI元素)
    std::unique_ptr<UiPanel> m_rootElement;
};

} // namespace engine::ui
