#include "title_scene.h"
#include "../data/session_data.h"
#include "game_scene.h"
#include "help_scene.h"

#include "../../engine/audio/audio_player.h"
#include "../../engine/core/context.h"
#include "../../engine/core/game_state.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/render/camera.h"
#include "../../engine/resource/resource_manager.h"
#include "../../engine/scene/level_loader.h"
#include "../../engine/scene/scene_manager.h"
#include "../../engine/ui/ui_button.h"
#include "../../engine/ui/ui_image.h"
#include "../../engine/ui/ui_label.h"
#include "../../engine/ui/ui_manager.h"
#include "../../engine/ui/ui_panel.h"

#include <glm/vec2.hpp>
#include <spdlog/spdlog.h>

namespace game::scene {

TitleScene::TitleScene(engine::core::Context& context,
                       engine::scene::SceneManager& sceneManager,
                       std::shared_ptr<game::data::SessionData> gameSessionData)
    : SceneBase{ "TitleScene", context, sceneManager }
    , m_gameSessionData{ std::move(gameSessionData) }
{
    if (m_gameSessionData == nullptr) {
        // 如果没有传入 SessionData，创建一个默认的
        m_gameSessionData = std::make_shared<game::data::SessionData>();
        spdlog::info("未提供 SessionData，使用默认值。");
    }
    spdlog::trace("TitleScene 构造完成。");
}

void TitleScene::init()
{
    if (m_isInitialized) {
        spdlog::warn("TitleScene 已初始化，重复调用 init()");
        return;
    }

    spdlog::trace("TitleScene 初始化开始...");

    m_context.gameState().setCurrentState(engine::core::State::InTitle);
    // 同步最高分
    m_gameSessionData->syncHighestScore("assets/save.json");

    // 重置相机坐标，不限制边界
    m_context.camera().setPosition(glm::vec2(0.0F));
    // 若无这一行，从GameScene返回到标题场景时，相机会限制在地图边界内
    m_context.camera().setLimitBounds(std::nullopt);

    // 加载背景
    engine::scene::LevelLoader levelLoader;
    if (!levelLoader.loadLevel("assets/maps/level0.tmj", *this)) {
        spdlog::error("加载背景失败。");
        return;
    }

    if (!initUi()) {
        spdlog::error("初始化 UI 失败，无法继续。");
        m_context.inputManager().setShouldQuit(true);
        return;
    }

    // 播放背景音乐
    m_context.audioPlayer().playMusic("assets/audio/platformer_level03_loop.ogg");

    SceneBase::init();
    spdlog::trace("TitleScene 初始化完成。");
}

void TitleScene::update(float deltaTime)
{
    SceneBase::update(deltaTime);

    // 相机自动向右移动
    m_context.camera().move(glm::vec2{ deltaTime * 100.0F, 0.0F });
}

bool TitleScene::initUi()
{
    spdlog::trace("创建 TitleScene UI...");
    glm::vec2 windowSize{ m_context.gameState().logicalSize() };

    if (!m_uiManager->init(windowSize)) {
        spdlog::error("TitleScene 中初始化 UiManager 失败!");
        return false;
    }

    // --- 创建标题图片 (假设不知道大小) ---
    auto titleImage = std::make_unique<engine::ui::UiImage>("assets/textures/UI/title-screen.png");
    auto size = m_context.resourceManager().getTextureSize(titleImage->textureId());
    titleImage->setSize(size * 2.0F); // 放大为2倍

    // 水平居中
    auto titleLocalPos = (windowSize - titleImage->size()) / 2.0F - glm::vec2{ 0.0F, 50.0F };
    titleImage->setLocalPosition(titleLocalPos);

    // 将标题图片添加到UI管理器
    m_uiManager->addElement(std::move(titleImage));

    // --- 创建按钮面板并居中 --- (4个按钮，设定好大小、间距)
    float buttonWidth{ 96.0F };
    float buttonHeight{ 32.0F };
    float buttonSpacing{ 20.0F };
    int buttonNums{ 4 };

    // 计算面板总宽度
    float panelWidth{ buttonNums * buttonWidth + (buttonNums - 1) * buttonSpacing };
    float panelHeight{ buttonHeight };

    // 计算面板位置使其居中
    float panelX{ (windowSize.x - panelWidth) / 2.0F };
    float panelY{ windowSize.y * 0.65F }; // 垂直位置中间靠下

    auto buttonPanel = std::make_unique<engine::ui::UiPanel>(glm::vec2{ panelX, panelY },
                                                             glm::vec2{ panelWidth, panelHeight });

    // --- 创建按钮并添加到 UiPanel (位置是相对于 UiPanel 左上角的局部位置) ---
    glm::vec2 currentButtonLocalPos{ 0.0F, 0.0F };
    glm::vec2 buttonSize{ buttonWidth, buttonHeight };

    // Start Button
    auto startButton
        = std::make_unique<engine::ui::UiButton>(m_context,
                                                 "assets/textures/UI/buttons/Start1.png",
                                                 "assets/textures/UI/buttons/Start2.png",
                                                 "assets/textures/UI/buttons/Start3.png",
                                                 currentButtonLocalPos,
                                                 buttonSize,
                                                 [this]() { this->startGame(); });
    buttonPanel->addChild(std::move(startButton));

    // Load Button
    currentButtonLocalPos.x += buttonWidth + buttonSpacing;
    auto loadButton = std::make_unique<engine::ui::UiButton>(m_context,
                                                             "assets/textures/UI/buttons/Load1.png",
                                                             "assets/textures/UI/buttons/Load2.png",
                                                             "assets/textures/UI/buttons/Load3.png",
                                                             currentButtonLocalPos,
                                                             buttonSize,
                                                             [this]() { this->loadGame(); });
    buttonPanel->addChild(std::move(loadButton));

    // Help Button
    currentButtonLocalPos.x += buttonWidth + buttonSpacing;
    auto helpButton
        = std::make_unique<engine::ui::UiButton>(m_context,
                                                 "assets/textures/UI/buttons/Helps1.png",
                                                 "assets/textures/UI/buttons/Helps2.png",
                                                 "assets/textures/UI/buttons/Helps3.png",
                                                 currentButtonLocalPos,
                                                 buttonSize,
                                                 [this]() { this->help(); });
    buttonPanel->addChild(std::move(helpButton));

    // Quit Button
    currentButtonLocalPos.x += buttonWidth + buttonSpacing;
    auto quitButton = std::make_unique<engine::ui::UiButton>(m_context,
                                                             "assets/textures/UI/buttons/Quit1.png",
                                                             "assets/textures/UI/buttons/Quit2.png",
                                                             "assets/textures/UI/buttons/Quit3.png",
                                                             currentButtonLocalPos,
                                                             buttonSize,
                                                             [this]() { this->quit(); });
    buttonPanel->addChild(std::move(quitButton));

    // 将 UiPanel 添加到UI管理器
    m_uiManager->addElement(std::move(buttonPanel));

    // --- 创建 Credit 标签 ---
    auto creditLabel = std::make_unique<engine::ui::UiLabel>(m_context.textRenderer(),
                                                             "SunnyLand Credits: XXX - 2026",
                                                             "assets/fonts/VonwaonBitmap-16px.ttf",
                                                             16,
                                                             engine::utils::FColor{
                                                                 0.8F, 0.8F, 0.8F, 1.0F });
    // 设置 Credit 标签位置，使其居中并靠下
    creditLabel->setLocalPosition(glm::vec2{ (windowSize.x - creditLabel->size().x) / 2.0F,
                                             windowSize.y - creditLabel->size().y - 10.0F });
    // 将 Credit 标签添加到UI管理器
    m_uiManager->addElement(std::move(creditLabel));

    spdlog::trace("TitleScene UI 创建完成.");
    return true;
}

void TitleScene::startGame()
{
    spdlog::debug("开始游戏按钮被点击。");

    // 开始新游戏会重置游戏会话数据
    if (m_gameSessionData != nullptr) {
        m_gameSessionData->reset();
    }

    // 替换为游戏场景
    m_sceneManager.requestReplaceScene(
        std::make_unique<GameScene>(m_context, m_sceneManager, m_gameSessionData));
}

void TitleScene::loadGame()
{
    spdlog::debug("加载游戏按钮被点击。");

    if (m_gameSessionData == nullptr) {
        spdlog::error("游戏状态为空，无法加载。");
        return;
    }

    if (m_gameSessionData->loadFromFile("assets/save.json")) {
        spdlog::debug("加载存档成功。开始游戏...");
        m_sceneManager.requestReplaceScene(
            std::make_unique<GameScene>(m_context, m_sceneManager, m_gameSessionData));
    } else {
        spdlog::warn("加载存档失败。");
    }
}

void TitleScene::help()
{
    spdlog::debug("帮助按钮被点击。");

    m_sceneManager.requestPushScene(std::make_unique<HelpScene>(m_context, m_sceneManager));
}

void TitleScene::quit()
{
    spdlog::debug("退出按钮被点击。");

    m_context.inputManager().setShouldQuit(true);
}

} // namespace game::scene
