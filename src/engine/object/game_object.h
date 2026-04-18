#pragma once

#include "../component/component_base.h"

#include <spdlog/spdlog.h>

#include <concepts>
#include <memory>
#include <typeindex> // 用于类型索引
#include <unordered_map>
#include <utility> // 用于完美转发

namespace engine::object {

template<typename T>
concept ComponentType = std::derived_from<T, engine::component::ComponentBase>;

/**
 * @brief 游戏对象类，负责管理游戏对象的组件。
 * 
 * 该类管理游戏对象的组件，并提供添加、获取、检查和移除组件的功能。
 * 它还提供更新和渲染游戏对象的方法。
 */
class GameObject final
{
public:
    ///< @brief 构造函数。默认名称为空，标签为空
    explicit GameObject(const std::string& name = "", const std::string& tag = "");

    // 禁止拷贝和移动，确保唯一性 (通常游戏对象不应随意拷贝)
    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;
    GameObject(GameObject&&) = delete;
    GameObject& operator=(GameObject&&) = delete;

    // getters and setters
    void setName(const std::string& name) { m_name = name; } ///< @brief 设置名称
    const std::string& name() const { return m_name; }       ///< @brief 获取名称
    void setTag(const std::string& tag) { m_tag = tag; }     ///< @brief 设置标签
    const std::string& tag() const { return m_tag; }         ///< @brief 获取标签
    ///< @brief 设置是否需要删除
    void setShouldRemove(bool shouldRemove) { m_shouldRemove = shouldRemove; }
    bool shouldRemove() const { return m_shouldRemove; } ///< @brief 获取是否需要删除

    /**
     * @brief 添加组件 (里面会完成组件的 init())
     * 
     * @tparam T 组件类型
     * @tparam Args 组件构造函数参数类型
     * @param args 组件构造函数参数
     * @return 组件指针
     */
    template<ComponentType T, typename... Args>
    T* addComponent(Args&&... args)
    {
        // 获取类型标识。
        /* typeid(T) -- 用于获取一个表达式或类型的运行时类型信息 (RTTI), 返回 std::type_info& */
        /* std::type_index -- 针对std::type_info对象的包装器，主要设计用来作为关联容器（如 std::map）的键。*/
        auto typeIndex = std::type_index{ typeid(T) };

        // 如果组件已经存在，则直接返回组件指针
        if (hasComponent<T>()) {
            return getComponent<T>();
        }

        // 如果不存在则创建组件
        /* std::forward -- 用于实现完美转发。传递多个参数的时候使用...标识 */
        auto newComponent = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr{ newComponent.get() }; // 先获取裸指针以便返回
        newComponent->setOwner(this); // 设置组件的拥有者
        /* newComponent 移动后变为空，不可再使用。因此必须使用 ptr 来调用组件的 init() 方法 */
        m_components[typeIndex] = std::move(newComponent); // 移动组件到容器中
        ptr->init();                                       // 初始化组件

        spdlog::debug("GameObject::addComponent: {} added component {}", m_name, typeid(T).name());
        return ptr; // 返回非拥有指针
    }

    /**
     * @brief 获取组件
     * 
     * @tparam T 组件类型
     * @return 组件指针
     */
    template<ComponentType T>
    T* getComponent() const
    {
        auto typeIndex = std::type_index{ typeid(T) };
        auto it = m_components.find(typeIndex);
        if (it != m_components.end()) {
            // 返回unique_ptr的裸指针。
            return static_cast<T*>(it->second.get());
        }
        return nullptr;
    }

    /**
     * @brief 检查是否存在组件
     * 
     * @tparam T 组件类型
     * @return 是否存在组件
     */
    template<ComponentType T>
    bool hasComponent() const
    {
        // contains方法为 C++20 新增
        return m_components.contains(std::type_index{ typeid(T) });
    }

    /**
     * @brief 移除组件
     * 
     * @tparam T 组件类型
     */
    template<ComponentType T>
    void removeComponent()
    {
        auto typeIndex = std::type_index{ typeid(T) };
        auto it = m_components.find(typeIndex);
        if (it != m_components.end()) {
            it->second->clean();
            m_components.erase(it);
        }
    }

    // 关键循环函数
    void handleInput(engine::core::Context& context);              ///< @brief 处理输入
    void update(double deltaTime, engine::core::Context& context); ///< @brief 更新所有组件
    void render(engine::core::Context& context);                   ///< @brief 渲染所有组件
    void clean();                                                  ///< @brief 清理所有组件

private:
    std::unordered_map<std::type_index, std::unique_ptr<engine::component::ComponentBase>>
        m_components;             ///< @brief 组件列表
    std::string m_name;           ///< @brief 名称
    std::string m_tag;            ///< @brief 标签
    bool m_shouldRemove{ false }; ///< @brief 延迟删除的标识，将来由场景类负责删除
};

} // namespace engine::object
