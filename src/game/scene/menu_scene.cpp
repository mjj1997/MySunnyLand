#include "menu_scene.h"
#include "../data/session_data.h"

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
        // 如果没有传入 SessionData，创建一个默认的
        m_gameSessionData = std::make_shared<game::data::SessionData>();
        spdlog::info("未提供 SessionData，使用默认值。");
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
    auto labelLocalPosY{ windowSize.y * 0.2f };
    pauseLabel->setLocalPosition(
        glm::vec2{ (windowSize.x - pauseLabel->size().x) / 2.0f, labelLocalPosY });
    // 将 PAUSE 标签添加到 UI 管理器
    m_uiManager->addElement(std::move(pauseLabel));

    // --- 创建 4 个按钮 --- (4个按钮，设定好大小、间距)
    float buttonWidth{ 96.0f };
    float buttonHeight{ 32.0f };
    float buttonSpacing{ 10.0f };

    glm::vec2 currentButtonLocalPos{
        (windowSize.x - buttonWidth) / 2.0f, // 按钮水平居中位置
        labelLocalPosY + 80.0f               // 从标签下方开始，增加间距
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

    spdlog::trace("MenuScene UI 创建完成.");
    return true;
}

void MenuScene::resume()
{
    spdlog::debug("继续游戏按钮被点击。");
    m_sceneManager.requestPopScene(); // 弹出当前场景
    m_context.gameState().setCurrentState(engine::core::State::Playing);
}

} // namespace game::scene
