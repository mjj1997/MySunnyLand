#pragma once

#include <memory>

struct SDL_Window;
struct SDL_Renderer;

namespace engine::resource {
class ResourceManager;
}

namespace engine::core { // 命名空间的最佳实践：与文件路径一致

class FrameTimeController;

/**
 * @brief 主游戏应用程序类，初始化SDL，管理游戏循环。
 */
class GameApp final // final 表示该类不能被继承
{
public:
    GameApp() = default;
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
    bool initSDL();
    bool initFrameTimeController();
    bool initResourceManager();

    // 测试用函数
    void testResourceManager();

private:
    SDL_Window* m_window{ nullptr };
    SDL_Renderer* m_renderer{ nullptr };
    bool m_isRunning{ false };

    // 引擎组件
    std::unique_ptr<engine::core::FrameTimeController> m_frameTimeController;
    std::unique_ptr<engine::resource::ResourceManager> m_resourceManager;
};

} // namespace engine::core
