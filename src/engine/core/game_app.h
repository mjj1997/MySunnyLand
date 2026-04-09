#pragma once

#include <memory>

struct SDL_Window;
struct SDL_Renderer;

namespace engine::resource {
class ResourceManager;
}

namespace engine::render {
class Renderer;
class Camera;
} // namespace engine::render

namespace engine::core { // 命名空间的最佳实践：与文件路径一致

class FrameTimeController;

/**
 * @brief 主游戏应用程序类，初始化SDL，管理游戏循环。
 */
class GameApp final // final 表示该类不能被继承
{
public:
    GameApp();
    ~GameApp();

    // 禁止拷贝和移动
    GameApp(const GameApp&) = delete;
    GameApp& operator=(const GameApp&) = delete;
    GameApp(GameApp&&) = delete;
    GameApp& operator=(GameApp&&) = delete;

    /**
     * @brief 运行游戏应用程序，其中会调用 init()，然后进入主循环，离开循环后自动调用 clean()。
     */
    void run();

private:
    [[nodiscard]] bool init(); // nodiscard属性 表示该函数返回值不应该被忽略
    void handleEvents();
    void update(double deltaTime);
    void render();
    void clean();

    // 各模块的初始化/创建函数，在 init() 中调用
    [[nodiscard]] bool initSDL();
    [[nodiscard]] bool initFrameTimeController();
    [[nodiscard]] bool initResourceManager();
    [[nodiscard]] bool initRenderer();
    [[nodiscard]] bool initCamera();

    // 测试用函数
    void testResourceManager();
    void testRenderer();
    void testCamera();

private:
    SDL_Window* m_window{ nullptr };
    SDL_Renderer* m_sdlRenderer{ nullptr };
    bool m_isRunning{ false };

    // 引擎组件
    std::unique_ptr<engine::core::FrameTimeController> m_frameTimeController;
    std::unique_ptr<engine::resource::ResourceManager> m_resourceManager;
    std::unique_ptr<engine::render::Renderer> m_renderer;
    std::unique_ptr<engine::render::Camera> m_camera;
};

} // namespace engine::core
