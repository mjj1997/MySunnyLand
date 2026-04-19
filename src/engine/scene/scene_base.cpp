#include "scene_base.h"
#include "../object/game_object.h"

#include <spdlog/spdlog.h>

namespace engine::scene {

SceneBase::SceneBase(std::string name, engine::core::Context& context)
    : m_sceneName{ std::move(name) }
    , m_context{ context }
    , m_isInitialized{ false }
{
    spdlog::trace("场景 '{}' 构造完成。", m_sceneName);
}

SceneBase::~SceneBase() = default;

void SceneBase::init()
{
    m_isInitialized = true; // 子类应该最后调用父类的 init 方法
    spdlog::trace("场景 '{}' 初始化完成。", m_sceneName);
}

void SceneBase::update(double deltaTime)
{
    if (!m_isInitialized)
        return;

    // 更新所有游戏对象，并删除需要移除的对象
    for (auto it = m_gameObjects.begin(); it != m_gameObjects.end();) {
        if (*it && !(*it)->shouldRemove()) { // 正常更新游戏对象
            (*it)->update(deltaTime, m_context);
            ++it;
        } else {
            if (*it) { // 安全删除需要移除的对象
                (*it)->clean();
            }
            it = m_gameObjects.erase(it); // 删除需要移除的对象，智能指针自动管理内存
        }
    }
}

void SceneBase::render()
{
    if (!m_isInitialized)
        return;

    // 渲染所有游戏对象
    for (const auto& obj : m_gameObjects) {
        if (obj) {
            obj->render(m_context);
        }
    }
}

void SceneBase::handleInput()
{
    if (!m_isInitialized)
        return;

    // 遍历所有游戏对象，略过需要移除的对象
    for (const auto& obj : m_gameObjects) {
        if (obj && !obj->shouldRemove()) {
            obj->handleInput(m_context);
        }
    }
}

void SceneBase::clean()
{
    if (!m_isInitialized)
        return;

    for (const auto& obj : m_gameObjects) {
        if (obj) {
            obj->clean();
        }
    }
    m_gameObjects.clear();

    m_isInitialized = false; // 清理完成后，设置场景为未初始化
    spdlog::trace("场景 '{}' 清理完成。", m_sceneName);
}

} // namespace engine::scene
