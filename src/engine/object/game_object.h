#pragma once

#include "../component/component_base.h"

#include <concepts>
#include <memory>
#include <typeindex> // 用于类型索引
#include <unordered_map>
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
    ///< @brief 构造函数。
    explicit GameObject();

    // 禁止拷贝和移动，确保唯一性 (通常游戏对象不应随意拷贝)
    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;
    GameObject(GameObject&&) = delete;
    GameObject& operator=(GameObject&&) = delete;

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

private:
    std::unordered_map<std::type_index, std::unique_ptr<engine::component::ComponentBase>>
        m_components;             ///< @brief 组件列表
};

} // namespace engine::object
