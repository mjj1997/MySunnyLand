#pragma once

#include <memory>
#include <string>
#include <vector>

namespace engine::core {
class Context;
}

namespace engine::object {
class GameObject;
}

namespace engine::scene {

/**
 * @brief 场景基类，负责管理场景中的游戏对象和场景生命周期。
 *
 * 包含一组游戏对象，并提供更新、渲染、处理输入和清理的接口。
 * 派生类应实现具体的场景逻辑。
 */
class SceneBase
{
public:
    /**
     * @brief 构造函数。
     *
     * @param name 场景的名称。
     * @param context 场景上下文。
     */
    SceneBase(std::string name, engine::core::Context& context);

    // 1. 基类必须声明虚析构函数才能让派生类析构函数被正确调用。
    // 2. 析构函数定义必须写在cpp中，不然需要引入GameObject头文件
    virtual ~SceneBase();

    // 禁止拷贝和移动构造
    SceneBase(const SceneBase&) = delete;
    SceneBase& operator=(const SceneBase&) = delete;
    SceneBase(SceneBase&&) = delete;
    SceneBase& operator=(SceneBase&&) = delete;

    // 核心循环方法
    virtual void init();                   ///< @brief 初始化场景。
    virtual void update(double deltaTime); ///< @brief 更新场景。
    virtual void render();                 ///< @brief 渲染场景。
    virtual void handleInput();            ///< @brief 处理输入。
    virtual void clean();                  ///< @brief 清理场景。

    // getters and setters
    void setName(const std::string& name) { m_sceneName = name; } ///< @brief 设置场景名称
    const std::string& name() const { return m_sceneName; }       ///< @brief 获取场景名称
    ///< @brief 设置场景是否已初始化
    void setInitialized(bool initialized) { m_isInitialized = initialized; }
    bool isInitialized() const { return m_isInitialized; } ///< @brief 获取场景是否已初始化

    engine::core::Context& context() const { return m_context; } ///< @brief 获取上下文引用
    /// @brief 获取场景中的游戏对象容器。
    const std::vector<std::unique_ptr<engine::object::GameObject>>& gameObjects() const
    {
        return m_gameObjects;
    }

protected:
    std::string m_sceneName;          ///< @brief 场景名称
    engine::core::Context& m_context; ///< @brief 上下文引用（构造时传入）
    ///< @brief 场景是否已初始化(非当前场景很可能未被删除，因此需要初始化标志避免重复初始化)
    bool m_isInitialized{ false };
    ///< @brief 场景中的游戏对象
    std::vector<std::unique_ptr<engine::object::GameObject>> m_gameObjects;
};

} // namespace engine::scene
