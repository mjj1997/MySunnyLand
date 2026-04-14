#include "game_object.h"

#include <spdlog/spdlog.h>

namespace engine::object {

GameObject::GameObject()
{
    spdlog::trace("GameObject created.");
}

void GameObject::handleInput()
{
    // 遍历所有组件并调用它们的 handleInput 方法
    for (auto& componentPair : m_components) {
        componentPair.second->handleInput();
    }
}

void GameObject::update(double deltaTime)
{
    // 遍历所有组件并调用它们的 update 方法
    for (auto& componentPair : m_components) {
        componentPair.second->update(deltaTime);
    }
}

void GameObject::render()
{
    // 遍历所有组件并调用它们的 render 方法
    for (auto& componentPair : m_components) {
        componentPair.second->render();
    }
}

void GameObject::clean()
{
    spdlog::trace("Cleaning GameObject...");
    // 遍历所有组件并调用它们的 clean 方法
    for (auto& componentPair : m_components) {
        componentPair.second->clean();
    }
    // 清理组件容器
    m_components.clear();
}

} // namespace engine::object
