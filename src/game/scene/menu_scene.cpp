#include "menu_scene.h"
#include "../data/session_data.h"
#include "title_scene.h"

#include "../../engine/core/context.h"
#include "../../engine/core/game_state.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/scene/scene_manager.h"
#include "../../engine/ui/ui_button.h"
#include "../../engine/ui/ui_label.h"
#include "../../engine/ui/ui_manager.h"

#include <spdlog/spdlog.h>

namespace game::scene {

MenuScene::MenuScene(engine::core::Context& context,
                     engine::scene::SceneManager& sceneManager,
                     std::shared_ptr<game::data::SessionData> gameSessionData)
    : SceneBase{ "MenuScene", context, sceneManager }
    , m_gameSessionData{ std::move(gameSessionData) }
{
    if (m_gameSessionData == nullptr) {
        spdlog::error("错误：菜单场景收到了空的游戏数据！");
    }
    spdlog::trace("MenuScene 构造完成。");
}

void MenuScene::init()
{
    if (m_isInitialized) {
        spdlog::warn("MenuScene 已初始化，重复调用 init()");
        return;
    }

    spdlog::trace("MenuScene 初始化开始...");

    m_context.gameState().setCurrentState(engine::core::State::Paused);

    if (!initUi()) {
        spdlog::error("初始化 UI 失败，无法继续。");
        m_context.inputManager().setShouldQuit(true);
        return;
    }

    SceneBase::init();
    spdlog::trace("MenuScene 初始化完成。");
}

void MenuScene::handleInput()
{
    // 先让 UiManager 处理交互
    SceneBase::handleInput();

    // 检查暂停键，允许按暂停键恢复游戏
    if (m_context.inputManager().isActionPressed("pause")) {
        spdlog::debug("在菜单场景中按下暂停键，正在恢复游戏...");
        m_sceneManager.requestPopScene(); // 弹出自身以恢复底层的 GameScene
        m_context.gameState().setCurrentState(engine::core::State::Playing);
    }
}

bool MenuScene::initUi()
{
    spdlog::trace("创建 MenuScene UI...");
    glm::vec2 windowSize{ m_context.gameState().logicalSize() };

    if (!m_uiManager->init(windowSize)) {
        spdlog::error("MenuScene 中初始化 UiManager 失败!");
        return false;
    }

    // --- 创建 PAUSE 标签 ---
    auto pauseLabel = std::make_unique<engine::ui::UiLabel>(m_context.textRenderer(),
                                                            "PAUSE",
                                                            "assets/fonts/VonwaonBitmap-16px.ttf",
                                                            32);
    // 设置 PAUSE 标签位置，使其居中并靠上
    auto labelLocalPosY{ windowSize.y * 0.2F };
    pauseLabel->setLocalPosition(
        glm::vec2{ (windowSize.x - pauseLabel->size().x) / 2.0F, labelLocalPosY });
    // 将 PAUSE 标签添加到 UI 管理器
    m_uiManager->addElement(std::move(pauseLabel));

    // --- 创建 4 个按钮 --- (4个按钮，设定好大小、间距)
    float buttonWidth{ 96.0F };
    float buttonHeight{ 32.0F };
    float buttonSpacing{ 10.0F };

    glm::vec2 currentButtonLocalPos{
        (windowSize.x - buttonWidth) / 2.0F, // 按钮水平居中位置
        labelLocalPosY + 80.0F               // 从标签下方开始，增加间距
    };
    glm::vec2 buttonSize{ buttonWidth, buttonHeight };

    // Resume Button
    auto resumeButton
        = std::make_unique<engine::ui::UiButton>(m_context,
                                                 "assets/textures/UI/buttons/Resume1.png",
                                                 "assets/textures/UI/buttons/Resume2.png",
                                                 "assets/textures/UI/buttons/Resume3.png",
                                                 currentButtonLocalPos,
                                                 buttonSize,
                                                 [this]() { this->resume(); });
    m_uiManager->addElement(std::move(resumeButton));

    // Save Button
    currentButtonLocalPos.y += buttonHeight + buttonSpacing;
    auto saveButton = std::make_unique<engine::ui::UiButton>(m_context,
                                                             "assets/textures/UI/buttons/Save1.png",
                                                             "assets/textures/UI/buttons/Save2.png",
                                                             "assets/textures/UI/buttons/Save3.png",
                                                             currentButtonLocalPos,
                                                             buttonSize,
                                                             [this]() { this->save(); });
    m_uiManager->addElement(std::move(saveButton));

    // Back Button
    currentButtonLocalPos.y += buttonHeight + buttonSpacing;
    auto backButton = std::make_unique<engine::ui::UiButton>(m_context,
                                                             "assets/textures/UI/buttons/Back1.png",
                                                             "assets/textures/UI/buttons/Back2.png",
                                                             "assets/textures/UI/buttons/Back3.png",
                                                             currentButtonLocalPos,
                                                             buttonSize,
                                                             [this]() { this->back(); });
    m_uiManager->addElement(std::move(backButton));

    // Quit Button
    currentButtonLocalPos.y += buttonHeight + buttonSpacing;
    auto quitButton = std::make_unique<engine::ui::UiButton>(m_context,
                                                             "assets/textures/UI/buttons/Quit1.png",
                                                             "assets/textures/UI/buttons/Quit2.png",
                                                             "assets/textures/UI/buttons/Quit3.png",
                                                             currentButtonLocalPos,
                                                             buttonSize,
                                                             [this]() { this->quit(); });
    m_uiManager->addElement(std::move(quitButton));

    spdlog::trace("MenuScene UI 创建完成.");
    return true;
}

void MenuScene::resume()
{
    spdlog::debug("继续游戏按钮被点击。");
    m_sceneManager.requestPopScene(); // 弹出当前场景
    m_context.gameState().setCurrentState(engine::core::State::Playing);
}

void MenuScene::save()
{
    spdlog::debug("保存游戏按钮被点击。");
    // 同步最高分
    m_gameSessionData->syncHighestScore("assets/save.json");
    if (m_gameSessionData->saveToFile("assets/save.json")) {
        spdlog::debug("菜单场景中成功保存游戏数据。");
    } else {
        spdlog::error("菜单场景中保存游戏数据失败。");
    }
}

void MenuScene::back()
{
    spdlog::debug("返回按钮被点击。弹出菜单场景和游戏场景，返回标题界面。");
    // 直接替换为TitleScene
    m_sceneManager.requestReplaceScene(
        std::make_unique<TitleScene>(m_context, m_sceneManager, m_gameSessionData));
}

void MenuScene::quit()
{
    spdlog::debug("退出按钮被点击。请求应用程序退出。");
    // 同步最高分
    m_gameSessionData->syncHighestScore("assets/save.json");
    m_context.inputManager().setShouldQuit(true); // 输入管理器设置退出标志
}

} // namespace game::scene
