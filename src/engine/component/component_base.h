#pragma once

namespace engine::core {
class Context;
}

namespace engine::object {
class GameObject;
}

namespace engine::component {

/**
 * @brief 所有游戏组件的抽象基类。
 *
 * 所有具体组件都应从此类继承。
 * 定义了组件生命周期中可能调用的通用方法。
 */
class ComponentBase
{
    friend class engine::object::GameObject; // GameObject 需要调用 ComponentBase 的 protected 方法

public:
    ComponentBase() = default;
    virtual ~ComponentBase() = default; ///< @brief 虚析构函数确保正确清理派生类

    // 禁止拷贝和移动，组件通常不应被拷贝或移动（更改 m_owner 就相当于移动）
    ComponentBase(const ComponentBase&) = delete;
    ComponentBase& operator=(const ComponentBase&) = delete;
    ComponentBase(ComponentBase&&) = delete;
    ComponentBase& operator=(ComponentBase&&) = delete;

    ///< @brief 设置拥有此组件的 GameObject
    void setOwner(engine::object::GameObject* owner) { m_owner = owner; }
    ///< @brief 获取拥有此组件的 GameObject
    [[nodiscard]] engine::object::GameObject* owner() const { return m_owner; }

    // 关键循环函数，全部设为保护，只有 GameObject 可以调用 (未来改为 = 0 以实现抽象类)
protected:
    ///< @brief 保留两段初始化的机制，GameObject 添加组件时自动调用，不需要外部调用
    virtual void init() {}
    virtual void handleInput(engine::core::Context& context) {} ///< @brief 处理输入

    /**
     * @brief 更新组件状态(纯虚函数，必须在子类中重新实现)
     * @param deltaTime 距上一帧的时间间隔（秒）
     * @param context 引擎上下文对象，包含渲染器、输入管理器等
     */
    virtual void update(float deltaTime, engine::core::Context& context) = 0;

    virtual void render(engine::core::Context& context) {} ///< @brief 渲染
    virtual void clean() {}                                ///< @brief 清理

    engine::object::GameObject* m_owner{ nullptr }; ///< @brief 指向拥有此组件的 GameObject
};

} // namespace engine::component
