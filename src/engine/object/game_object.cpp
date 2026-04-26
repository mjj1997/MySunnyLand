#include "game_object.h"

#include <spdlog/spdlog.h>

namespace engine::object {

GameObject::GameObject(const std::string& name, const std::string& tag)
    : m_name{ name }
    , m_tag{ tag }
{
    spdlog::trace("GameObject created: {} {}", m_name, m_tag);
}

void GameObject::handleInput(engine::core::Context& context)
{
    // 遍历所有组件并调用它们的 handleInput 方法
    for (auto& [type, component] : m_components) {
        component->handleInput(context);
    }
}

void GameObject::update(float deltaTime, engine::core::Context& context)
{
    // 遍历所有组件并调用它们的 update 方法
    for (auto& [type, component] : m_components) {
        component->update(deltaTime, context);
    }
}

void GameObject::render(engine::core::Context& context)
{
    // 遍历所有组件并调用它们的 render 方法
    for (auto& [type, component] : m_components) {
        component->render(context);
    }
}

void GameObject::clean()
{
    spdlog::trace("Cleaning GameObject...");
    // 遍历所有组件并调用它们的 clean 方法
    for (auto& [type, component] : m_components) {
        component->clean();
    }
    // 清理组件容器
    m_components.clear();
}

} // namespace engine::object
