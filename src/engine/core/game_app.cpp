#include "game_app.h"
#include "../render/camera.h"
#include "../render/renderer.h"
#include "../render/sprite.h"
#include "../resource/resource_manager.h"
#include "configurator.h"
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

    m_frameTimeController->setTargetFps(144); // 设置目标帧率（临时，未来会从配置文件读取）
    while (m_isRunning) {
        m_frameTimeController->update();
        double deltaTime{ m_frameTimeController->deltaTime() };

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

    // 测试资源管理器
    testResourceManager();

    m_isRunning = true;
    spdlog::trace("GameApp 初始化成功。");
    return true;
}

void GameApp::handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            m_isRunning = false;
        }
    }
}

void GameApp::update(double deltaTime)
{
    testCamera();
}

void GameApp::render()
{
    // 1. 清除屏幕
    m_renderer->clearScreen();

    // 2. 具体渲染代码
    testRenderer();

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

    m_window = SDL_CreateWindow("SunnyLand", 1280, 720, SDL_WINDOW_RESIZABLE);
    if (m_window == nullptr) {
        spdlog::error("SDL 创建窗口失败！SDL 错误：{}", SDL_GetError());
        return false;
    }

    m_sdlRenderer = SDL_CreateRenderer(m_window, nullptr);
    if (m_sdlRenderer == nullptr) {
        spdlog::error("SDL 创建渲染器失败！SDL 错误：{}", SDL_GetError());
        return false;
    }

    // 设置逻辑分辨率
    SDL_SetRenderLogicalPresentation(m_sdlRenderer, 640, 360, SDL_LOGICAL_PRESENTATION_LETTERBOX);

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
        m_camera = std::make_unique<engine::render::Camera>(glm::vec2{ 640.0f, 360.0f });
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

void GameApp::testResourceManager()
{
    m_resourceManager->getTexture("assets/textures/Actors/eagle-attack.png");
    m_resourceManager->getSound("assets/audio/button_click.wav");
    m_resourceManager->getFont("assets/fonts/VonwaonBitmap-16px.ttf", 16);
    m_resourceManager->unloadTexture("assets/textures/Actors/eagle-attack.png");
    m_resourceManager->unloadSound("assets/audio/button_click.wav");
    m_resourceManager->unloadFont("assets/fonts/VonwaonBitmap-16px.ttf", 16);
}

void GameApp::testRenderer()
{
    engine::render::Sprite spriteWorld{ "assets/textures/Actors/frog.png" };
    engine::render::Sprite spriteUi{ "assets/textures/UI/buttons/Start1.png" };
    engine::render::Sprite spriteParallax{ "assets/textures/Layers/back.png" };

    static float rotation{ 0.0f };
    rotation += 0.1f;

    // 注意渲染顺序(先渲染背景, 再渲染角色, 最后渲染UI)
    m_renderer->drawParallax(*m_camera,
                             spriteParallax,
                             glm::vec2{ 100.0f, 100.0f },
                             glm::vec2{ 0.5f, 0.5f },
                             glm::bvec2{ true, false });
    m_renderer->drawSprite(*m_camera,
                           spriteWorld,
                           glm::vec2{ 200.0f, 200.0f },
                           glm::vec2{ 1.0f, 1.0f },
                           rotation);
    m_renderer->drawUiSprite(spriteUi, glm::vec2{ 100.0f, 100.0f });
}

void GameApp::testCamera()
{
    auto* keyState = SDL_GetKeyboardState(nullptr);
    if (keyState[SDL_SCANCODE_UP]) {
        m_camera->move(glm::vec2{ 0.0f, -1.0f });
    }
    if (keyState[SDL_SCANCODE_DOWN]) {
        m_camera->move(glm::vec2{ 0.0f, 1.0f });
    }
    if (keyState[SDL_SCANCODE_LEFT]) {
        m_camera->move(glm::vec2{ -1.0f, 0.0f });
    }
    if (keyState[SDL_SCANCODE_RIGHT]) {
        m_camera->move(glm::vec2{ 1.0f, 0.0f });
    }
}

} // namespace engine::core
