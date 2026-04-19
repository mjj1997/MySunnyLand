#pragma once

#include <memory>
#include <string>
#include <vector>

namespace engine::core {
class Context;
}

namespace engine::scene {
class SceneBase;

/**
 * @brief 管理游戏中的场景栈，处理场景切换和生命周期。
 */
class SceneManager final
{
public:
    explicit SceneManager(engine::core::Context& context);
    ~SceneManager();

    // 禁止拷贝和移动
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;
    SceneManager(SceneManager&&) = delete;
    SceneManager& operator=(SceneManager&&) = delete;

    // getters
    SceneBase* currentScene() const; ///< @brief 获取当前活动场景（栈顶场景）的指针。
    engine::core::Context& context() const { return m_context; } ///< @brief 获取引擎上下文引用。

    // 核心循环函数
    void update(double deltaTime);
    void render();
    void handleInput();
    void clean();

private:
    engine::core::Context& m_context;                     ///< @brief 引擎上下文引用
    std::vector<std::unique_ptr<SceneBase>> m_sceneStack; ///< @brief 场景栈
};

} // namespace engine::scene
