#pragma once

namespace engine::input {
class InputManager;
}

namespace engine::render {
class Renderer;
class Camera;
class TextRenderer;
} // namespace engine::render

namespace engine::resource {
class ResourceManager;
}

namespace engine::physics {
class PhysicsEngine;
}

namespace engine::audio {
class AudioPlayer;
}

namespace engine::core {
class GameState;

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
     * @param physicsEngine 对 PhysicsEngine 实例的引用。
     * @param audioPlayer 对 AudioPlayer 实例的引用。
     * @param textRenderer 对 TextRenderer 实例的引用。
     * @param gameState 对 GameState 实例的引用。
     * */
    Context(engine::input::InputManager& inputManager,
            engine::render::Renderer& renderer,
            engine::render::Camera& camera,
            engine::resource::ResourceManager& resourceManager,
            engine::physics::PhysicsEngine& physicsEngine,
            engine::audio::AudioPlayer& audioPlayer,
            engine::render::TextRenderer& textRenderer,
            GameState& gameState);

    // 禁止拷贝和移动，Context 对象通常是唯一的或按需创建/传递
    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;
    Context(Context&&) = delete;
    Context& operator=(Context&&) = delete;

    // --- Getters ---
    ///< @brief 获取输入管理器
    engine::input::InputManager& inputManager() const { return m_inputManager; }
    engine::render::Renderer& renderer() const { return m_renderer; } ///< @brief 获取渲染器
    engine::render::Camera& camera() const { return m_camera; }       ///< @brief 获取相机
    ///< @brief 获取资源管理器
    engine::resource::ResourceManager& resourceManager() const { return m_resourceManager; }
    ///< @brief 获取物理引擎
    engine::physics::PhysicsEngine& physicsEngine() const { return m_physicsEngine; }
    ///< @brief 获取音频播放器
    engine::audio::AudioPlayer& audioPlayer() const { return m_audioPlayer; }
    ///< @brief 获取文字渲染引擎
    engine::render::TextRenderer& textRenderer() const { return m_textRenderer; }
    ///< @brief 获取游戏状态
    GameState& gameState() const { return m_gameState; }

private:
    // 使用引用，确保每个模块都有效，使用时不需要检查指针是否为空。
    engine::input::InputManager& m_inputManager;          ///< @brief 输入管理器
    engine::render::Renderer& m_renderer;                 ///< @brief 渲染器
    engine::render::Camera& m_camera;                     ///< @brief 相机
    engine::resource::ResourceManager& m_resourceManager; ///< @brief 资源管理器
    engine::physics::PhysicsEngine& m_physicsEngine;      ///< @brief 物理引擎
    engine::audio::AudioPlayer& m_audioPlayer;            ///< @brief 音频播放器
    engine::render::TextRenderer& m_textRenderer;         ///< @brief 文字渲染引擎
    GameState& m_gameState;                               ///< @brief 游戏状态
};

} // namespace engine::core
