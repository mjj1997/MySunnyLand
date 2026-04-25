#include "game_app.h"
#include "../../game/scene/game_scene.h"
#include "../component/sprite_component.h"
#include "../component/transform_component.h"
#include "../input/input_manager.h"
#include "../object/game_object.h"
#include "../render/camera.h"
#include "../render/renderer.h"
#include "../render/sprite.h"
#include "../resource/resource_manager.h"
#include "../scene/scene_manager.h"
#include "configurator.h"
#include "context.h"
#include "frame_time_controller.h"

#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

namespace engine::core {

GameApp::GameApp() = default;

GameApp::~GameApp()
{
    if (m_isRunning) {
        spdlog::warn("GameApp 被销毁时没有显式关闭。现在关闭。 ...");
        clean();
    }
}

void GameApp::run()
{
    if (!init()) {
        spdlog::error("GameApp 初始化失败。");
        return;
    }

    while (m_isRunning) {
        m_frameTimeController->update();
        float deltaTime{ m_frameTimeController->deltaTime() };
        m_inputManager->update(); // 每帧首先更新输入管理器

        handleEvents();
        update(deltaTime);
        render();

        // spdlog::info("deltaTime: {}", deltaTime);
    }

    clean();
}

bool GameApp::init()
{
    spdlog::trace("初始化 GameApp ...");
    if (!initConfigurator()) {
        return false;
    }
    if (!initSDL()) {
        return false;
    }
    if (!initFrameTimeController()) {
        return false;
    }
    if (!initResourceManager()) {
        return false;
    }
    if (!initRenderer()) {
        return false;
    }
    if (!initCamera()) {
        return false;
    }
    if (!initInputManager()) {
        return false;
    }
    if (!initPhysicsEngine()) {
        return false;
    }
    if (!initContext()) {
        return false;
    }
    if (!initSceneManager()) {
        return false;
    }

    // 创建第一个场景并压入场景栈
    auto scene = std::make_unique<game::scene::GameScene>("GameScene", *m_context, *m_sceneManager);
    m_sceneManager->requestPushScene(std::move(scene));

    m_isRunning = true;
    spdlog::trace("GameApp 初始化成功。");

    return true;
}

void GameApp::handleEvents()
{
    if (m_inputManager->shouldQuit()) {
        spdlog::trace("GameApp 收到来自 InputManager 的退出信号。");
        m_isRunning = false;
        return;
    }

    m_sceneManager->handleInput();
}

void GameApp::update(float deltaTime)
{
    m_sceneManager->update(deltaTime);
}

void GameApp::render()
{
    // 1. 清除屏幕
    m_renderer->clearScreen();

    // 2. 具体渲染代码
    m_sceneManager->render();

    // 3. 更新屏幕显示
    m_renderer->present();
}

void GameApp::clean()
{
    spdlog::trace("关闭 GameApp ...");

    // 为了确保正确的销毁顺序，有些智能指针对象也需要手动管理
    m_resourceManager.reset();

    if (m_sdlRenderer != nullptr) {
        SDL_DestroyRenderer(m_sdlRenderer);
        m_sdlRenderer = nullptr;
    }
    if (m_window != nullptr) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
    SDL_Quit();
    m_isRunning = false;
}

bool GameApp::initSDL()
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        spdlog::error("SDL 初始化失败！SDL 错误：{}", SDL_GetError());
        return false;
    }

    m_window = SDL_CreateWindow(m_configurator->m_windowTitle.c_str(),
                                m_configurator->m_windowWidth,
                                m_configurator->m_windowHeight,
                                SDL_WINDOW_RESIZABLE);
    if (m_window == nullptr) {
        spdlog::error("SDL 创建窗口失败！SDL 错误：{}", SDL_GetError());
        return false;
    }

    m_sdlRenderer = SDL_CreateRenderer(m_window, nullptr);
    if (m_sdlRenderer == nullptr) {
        spdlog::error("SDL 创建渲染器失败！SDL 错误：{}", SDL_GetError());
        return false;
    }

    int vsyncMode{ m_configurator->m_isVSyncEnabled ? SDL_RENDERER_VSYNC_ADAPTIVE
                                                    : SDL_RENDERER_VSYNC_DISABLED };
    SDL_SetRenderVSync(m_sdlRenderer, vsyncMode);
    spdlog::trace("VSync 模式设置为: {}", m_configurator->m_isVSyncEnabled ? "启用" : "禁用");

    // 设置逻辑分辨率为窗口大小的一半（针对像素游戏）
    SDL_SetRenderLogicalPresentation(m_sdlRenderer,
                                     m_configurator->m_windowWidth / 2,
                                     m_configurator->m_windowHeight / 2,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);

    spdlog::trace("SDL 初始化成功。");
    return true;
}

bool GameApp::initFrameTimeController()
{
    try {
        m_frameTimeController = std::make_unique<FrameTimeController>();
    } catch (const std::exception& e) {
        spdlog::error("初始化帧时间管理器失败: {}", e.what());
        return false;
    }

    m_frameTimeController->setTargetFps(m_configurator->m_targetFps);

    spdlog::trace("帧时间管理器初始化成功。");
    return true;
}

bool GameApp::initResourceManager()
{
    try {
        m_resourceManager = std::make_unique<engine::resource::ResourceManager>(m_sdlRenderer);
    } catch (const std::exception& e) {
        spdlog::error("初始化资源管理器失败: {}", e.what());
        return false;
    }

    spdlog::trace("资源管理器初始化成功。");
    return true;
}

bool GameApp::initRenderer()
{
    try {
        m_renderer = std::make_unique<engine::render::Renderer>(m_sdlRenderer,
                                                                m_resourceManager.get());
    } catch (const std::exception& e) {
        spdlog::error("初始化渲染器失败: {}", e.what());
        return false;
    }
    spdlog::trace("渲染器初始化成功。");
    return true;
}

bool GameApp::initCamera()
{
    try {
        m_camera = std::make_unique<engine::render::Camera>(
            glm::vec2{ m_configurator->m_windowWidth / 2, m_configurator->m_windowHeight / 2 });
    } catch (const std::exception& e) {
        spdlog::error("初始化相机失败: {}", e.what());
        return false;
    }
    spdlog::trace("相机初始化成功。");
    return true;
}

bool GameApp::initConfigurator()
{
    try {
        m_configurator = std::make_unique<engine::core::Configurator>("assets/config.json");
    } catch (const std::exception& e) {
        spdlog::error("初始化配置管理器失败: {}", e.what());
        return false;
    }
    spdlog::trace("配置管理器初始化成功。");
    return true;
}

bool GameApp::initInputManager()
{
    try {
        m_inputManager = std::make_unique<engine::input::InputManager>(m_sdlRenderer,
                                                                       m_configurator.get());
    } catch (const std::exception& e) {
        spdlog::error("初始化输入管理器失败: {}", e.what());
        return false;
    }
    spdlog::trace("输入管理器初始化成功。");
    return true;
}

bool GameApp::initPhysicsEngine()
{
    try {
        m_physicsEngine = std::make_unique<engine::physics::PhysicsEngine>();
    } catch (const std::exception& e) {
        spdlog::error("初始化物理引擎失败: {}", e.what());
        return false;
    }
    spdlog::trace("物理引擎初始化成功。");
    return true;
}

bool GameApp::initContext()
{
    try {
        m_context = std::make_unique<engine::core::Context>(*m_inputManager,
                                                            *m_renderer,
                                                            *m_camera,
                                                            *m_resourceManager);
    } catch (const std::exception& e) {
        spdlog::error("初始化上下文失败: {}", e.what());
        return false;
    }
    spdlog::trace("上下文初始化成功。");
    return true;
}

bool GameApp::initSceneManager()
{
    try {
        m_sceneManager = std::make_unique<engine::scene::SceneManager>(*m_context);
    } catch (const std::exception& e) {
        spdlog::error("初始化场景管理器失败: {}", e.what());
        return false;
    }
    spdlog::trace("场景管理器初始化成功。");
    return true;
}

} // namespace engine::core
