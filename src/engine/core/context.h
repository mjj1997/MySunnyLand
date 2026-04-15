#pragma once

namespace engine::input {
class InputManager;
}

namespace engine::render {
class Renderer;
class Camera;
} // namespace engine::render

namespace engine::resource {
class ResourceManager;
}

namespace engine::core {

/**
 * @brief 持有对核心引擎模块引用的上下文对象。
 *
 * 用于简化依赖注入，传递Context即可获取引擎的各个模块。
 */
class Context final
{
public:
    /**
     * @brief 构造函数。
     * @param inputManager 对 InputManager 实例的引用。
     * @param renderer 对 Renderer 实例的引用。
     * @param camera 对 Camera 实例的引用。
     * @param resourceManager 对 ResourceManager 实例的引用。
     */
    Context(engine::input::InputManager& inputManager,
            engine::render::Renderer& renderer,
            engine::render::Camera& camera,
            engine::resource::ResourceManager& resourceManager);

    // 禁止拷贝和移动，Context 对象通常是唯一的或按需创建/传递
    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;
    Context(Context&&) = delete;
    Context& operator=(Context&&) = delete;

    // --- Getters ---
    engine::input::InputManager& inputManager() const ///< @brief 获取输入管理器
    {
        return m_inputManager;
    }
    engine::render::Renderer& renderer() const { return m_renderer; } ///< @brief 获取渲染器
    engine::render::Camera& camera() const { return m_camera; }       ///< @brief 获取相机
    engine::resource::ResourceManager& resourceManager() const        ///< @brief 获取资源管理器
    {
        return m_resourceManager;
    }

private:
    // 使用引用，确保每个模块都有效，使用时不需要检查指针是否为空。
    engine::input::InputManager& m_inputManager;          ///< @brief 输入管理器
    engine::render::Renderer& m_renderer;                 ///< @brief 渲染器
    engine::render::Camera& m_camera;                     ///< @brief 相机
    engine::resource::ResourceManager& m_resourceManager; ///< @brief 资源管理器
};

} // namespace engine::core
