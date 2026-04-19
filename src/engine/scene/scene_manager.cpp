#include "scene_manager.h"
#include "../core/context.h"
#include "scene_base.h"

#include <spdlog/spdlog.h>

namespace engine::scene {

SceneManager::SceneManager(engine::core::Context& context)
    : m_context{ context }
{
    spdlog::trace("场景管理器已创建。");
}

SceneManager::~SceneManager()
{
    spdlog::trace("场景管理器已销毁。");
    clean(); // 即使不手动调用 clean() 也能确保清理
}

SceneBase* SceneManager::currentScene() const
{
    if (m_sceneStack.empty()) {
        return nullptr;
    }

    return m_sceneStack.back().get(); // 返回栈顶场景的裸指针
}

void SceneManager::update(double deltaTime)
{
    // 只更新栈顶（当前）场景
    SceneBase* scene = currentScene();
    if (scene) {
        scene->update(deltaTime);
    }
}

void SceneManager::render()
{
    // 渲染时需要叠加渲染所有场景，而不只是栈顶
    for (const auto& scene : m_sceneStack) {
        if (scene) {
            scene->render();
        }
    }
}

void SceneManager::handleInput()
{
    // 只考虑栈顶（当前）场景
    SceneBase* scene = currentScene();
    if (scene) {
        scene->handleInput();
    }
}

void SceneManager::clean()
{
    spdlog::trace("正在关闭场景管理器并清理场景栈...");
    // 清理栈中所有剩余的场景（从顶到底）
    while (!m_sceneStack.empty()) {
        if (m_sceneStack.back()) {
            spdlog::debug("正在清理场景 '{}' 。", m_sceneStack.back()->name());
            m_sceneStack.back()->clean();
        }
        m_sceneStack.pop_back();
    }
}

} // namespace engine::scene
