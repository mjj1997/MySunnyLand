#include "game_app.h"
#include "../resource/resource_manager.h"
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

        spdlog::info("deltaTime: {}", deltaTime);
    }

    clean();
}

bool GameApp::init()
{
    spdlog::trace("初始化 GameApp ...");
    if (!initSDL()) {
        return false;
    }
    if (!initFrameTimeController()) {
        return false;
    }
    if (!initResourceManager()) {
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
    // 更新游戏状态, 暂时为空
}

void GameApp::render()
{
    // 渲染游戏状态, 暂时为空
}

void GameApp::clean()
{
    spdlog::trace("关闭 GameApp ...");

    // 为了确保正确的销毁顺序，有些智能指针对象也需要手动管理
    m_resourceManager.reset();

    if (m_renderer != nullptr) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
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

    m_renderer = SDL_CreateRenderer(m_window, nullptr);
    if (m_renderer == nullptr) {
        spdlog::error("SDL 创建渲染器失败！SDL 错误：{}", SDL_GetError());
        return false;
    }

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
        m_resourceManager = std::make_unique<engine::resource::ResourceManager>(m_renderer);
    } catch (const std::exception& e) {
        spdlog::error("初始化资源管理器失败: {}", e.what());
        return false;
    }

    spdlog::trace("资源管理器初始化成功。");
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

} // namespace engine::core
