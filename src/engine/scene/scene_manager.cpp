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

void SceneManager::update(float deltaTime)
{
    // 只更新栈顶（当前）场景
    SceneBase* scene = currentScene();
    if (scene) {
        scene->update(deltaTime);
    }

    // 执行可能的切换场景操作
    processPendingActions();
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

void SceneManager::requestPushScene(std::unique_ptr<SceneBase>&& scene)
{
    m_pendingAction = PendingAction::Push;
    m_pendingScene = std::move(scene);
}

void SceneManager::requestPopScene()
{
    m_pendingAction = PendingAction::Pop;
}

void SceneManager::requestReplaceScene(std::unique_ptr<SceneBase>&& scene)
{
    m_pendingAction = PendingAction::Replace;
    m_pendingScene = std::move(scene);
}

// --- Private Methods ---

void SceneManager::processPendingActions()
{
    if (m_pendingAction == PendingAction::None) {
        return;
    }

    switch (m_pendingAction) {
    case PendingAction::Push:
        pushScene(std::move(m_pendingScene));
        break;
    case PendingAction::Pop:
        popScene();
        break;
    case PendingAction::Replace:
        replaceScene(std::move(m_pendingScene));
        break;
    default:
        break;
    }

    m_pendingAction = PendingAction::None;
}

void SceneManager::pushScene(std::unique_ptr<SceneBase>&& scene)
{
    if (!scene) {
        spdlog::warn("尝试将空场景压入栈。");
        return;
    }

    spdlog::debug("正在将场景 '{}' 压入栈。", scene->name());

    // 初始化新场景
    if (!scene->isInitialized()) { // 确保只初始化一次
        scene->init();
    }

    // 将新场景移入栈顶
    m_sceneStack.push_back(std::move(scene));
}

void SceneManager::popScene()
{
    if (m_sceneStack.empty()) {
        spdlog::warn("尝试从空场景栈中弹出。");
        return;
    }

    spdlog::debug("正在从栈中弹出场景 '{}' 。", m_sceneStack.back()->name());

    // 清理并移除栈顶场景
    if (m_sceneStack.back()) {
        m_sceneStack.back()->clean(); // 显式调用清理
    }
    m_sceneStack.pop_back();
}

void SceneManager::replaceScene(std::unique_ptr<SceneBase>&& scene)
{
    if (!scene) {
        spdlog::warn("尝试用空场景替换。");
        return;
    }

    spdlog::debug("正在用场景 '{}' 替换场景 '{}' 。", scene->name(), m_sceneStack.back()->name());

    // 清理并移除场景栈中所有场景
    while (!m_sceneStack.empty()) {
        if (m_sceneStack.back()) {
            m_sceneStack.back()->clean();
        }
        m_sceneStack.pop_back();
    }

    // 初始化新场景
    if (!scene->isInitialized()) {
        scene->init();
    }

    // 将新场景压入栈顶
    m_sceneStack.push_back(std::move(scene));
}

} // namespace engine::scene
