#include "scene_base.h"
#include "../object/game_object.h"

#include <spdlog/spdlog.h>

#include <algorithm> // for std::remove_if

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

void SceneBase::addGameObject(std::unique_ptr<engine::object::GameObject>&& gameObject)
{
    if (gameObject) {
        m_gameObjects.push_back(std::move(gameObject));
    } else {
        spdlog::warn("尝试向场景 '{}' 添加空游戏对象。", m_sceneName);
    }
}

void SceneBase::removeGameObject(engine::object::GameObject* gameObject)
{
    if (!gameObject) {
        spdlog::warn("尝试从场景 '{}' 中移除一个空的游戏对象指针。", m_sceneName);
        return;
    }

    // erase-remove 移除法不可用，因为智能指针与裸指针无法比较
    // 需要使用 std::remove_if 和 lambda 表达式自定义比较的方式
    auto it = std::remove_if(m_gameObjects.begin(),
                             m_gameObjects.end(),
                             [gameObject](const std::unique_ptr<engine::object::GameObject>& p) {
                                 // 比较裸指针是否相等（自定义比较方式）
                                 return p.get() == gameObject;
                             });

    if (it != m_gameObjects.end()) {
        (*it)->clean(); // 因为传入的是指针，因此只可能有一个元素被移除，不需要遍历it到末尾
        m_gameObjects.erase(it, m_gameObjects.end()); // 删除从it到末尾的元素（最后一个元素）
        spdlog::trace("从场景 '{}' 中移除游戏对象。", m_sceneName);
    } else {
        spdlog::warn("游戏对象指针未找到在场景 '{}' 中。", m_sceneName);
    }
}

engine::object::GameObject* SceneBase::findGameObjectByName(const std::string& name) const
{
    // 找到第一个符合条件的游戏对象就返回
    for (const auto& obj : m_gameObjects) {
        if (obj && obj->name() == name) {
            return obj.get();
        }
    }
    return nullptr;
}

} // namespace engine::scene
