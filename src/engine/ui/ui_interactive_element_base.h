#pragma once

#include "../render/sprite.h" // 需要引入头文件而不是前置声明（map容器创建时可能会检查内部元素是否有析构定义）
#include "../utils/string_view_hash.h"
#include "state/ui_state_base.h"
#include "ui_element_base.h"

#include <string_view>
#include <unordered_map>

namespace engine::ui {

/**
 * @brief 可交互 UI 元素的基类，继承自 UiElementBase
 *
 * 定义了可交互 UI 元素的通用属性和行为。
 * 管理 UI 元素状态的切换和交互逻辑。
 * 提供事件处理、更新和渲染的虚方法。
 */
class UiInteractiveElementBase : public UiElementBase
{
public:
    /**
     * @brief 构造 UiInteractiveElementBase
     * @param context 引擎上下文
     * @param localPosition 初始局部位置
     * @param size 初始大小
     */
    explicit UiInteractiveElementBase(engine::core::Context& context,
                                      glm::vec2 localPosition = { 0.0F, 0.0F },
                                      glm::vec2 size = { 0.0F, 0.0F });

    ~UiInteractiveElementBase() override = default;

    // --- 禁用拷贝和移动语义 ---
    UiInteractiveElementBase(const UiInteractiveElementBase&) = delete;
    UiInteractiveElementBase& operator=(const UiInteractiveElementBase&) = delete;
    UiInteractiveElementBase(UiInteractiveElementBase&&) = delete;
    UiInteractiveElementBase& operator=(UiInteractiveElementBase&&) = delete;

    // --- 事件处理方法 ---
    virtual void clicked() {} ///< @brief 如果有点击事件，则重写该方法

    // --- 核心方法 ---
    bool handleInput(engine::core::Context& context) override;
    void render(engine::core::Context& context) override;

    void addSprite(std::string_view name,
                   std::unique_ptr<engine::render::Sprite> sprite); ///< @brief 添加状态名称-精灵对
    void addSound(std::string_view name, std::string_view path);    ///< @brief 添加状态名称-音效对
    void playSound(std::string_view name);                          ///< @brief 播放音效

    // --- Getters and Setters ---
    ///< @brief 设置当前状态
    void setCurrentState(std::unique_ptr<engine::ui::state::UiStateBase> state);
    ///< @brief 获取当前状态
    engine::ui::state::UiStateBase* currentState() const { return m_currentState.get(); }

    void setCurrentSprite(std::string_view name); ///< @brief 通过状态名称，设置当前显示的精灵

    ///< @brief 设置是否可交互
    void setInteractive(bool interactive) { m_isInteractive = interactive; }
    bool isInteractive() const { return m_isInteractive; } ///< @brief 获取是否可交互

protected:
    engine::core::Context& m_context; ///< @brief 可交互 UI 元素很可能需要其他引擎组件

    ///< @brief 状态和精灵的映射，key 为状态名称，value 为精灵指针
    std::unordered_map<std::string,
                       std::unique_ptr<engine::render::Sprite>,
                       engine::utils::StringViewHash,
                       std::equal_to<>>
        m_sprites;
    ///< @brief 状态和音效的映射，key 为状态名称，value 为音效文件路径
    std::unordered_map<std::string, std::string, engine::utils::StringViewHash, std::equal_to<>>
        m_sounds;

    std::unique_ptr<engine::ui::state::UiStateBase> m_currentState; ///< @brief 当前状态
    engine::render::Sprite* m_currentSprite{ nullptr };             ///< @brief 当前显示的精灵
    bool m_isInteractive{ true };                                   ///< @brief 是否可交互
};

} // namespace engine::ui
