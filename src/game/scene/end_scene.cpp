#include "end_scene.h"
#include "../data/session_data.h"
#include "game_scene.h"
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

EndScene::EndScene(engine::core::Context& context,
                   engine::scene::SceneManager& sceneManager,
                   std::shared_ptr<game::data::SessionData> gameSessionData)
    : SceneBase{ "EndScene", context, sceneManager }
    , m_gameSessionData{ std::move(gameSessionData) }
{
    if (m_gameSessionData == nullptr) {
        spdlog::error("错误：结束场景收到了空的游戏数据！");
    }
    spdlog::trace("EndScene (胜利：{}) 构造完成。", m_gameSessionData->isWin() ? "是" : "否");
}

void EndScene::init()
{
    if (m_isInitialized) {
        spdlog::warn("EndScene 已初始化，重复调用 init()");
        return;
    }

    spdlog::trace("EndScene 初始化开始...");

    m_context.gameState().setCurrentState(engine::core::State::GameOver);
    // 同步最高分
    m_gameSessionData->syncHighestScore("assets/save.json");

    if (!initUi()) {
        spdlog::error("初始化 UI 失败，无法继续。");
        m_context.inputManager().setShouldQuit(true);
        return;
    }

    SceneBase::init();
    spdlog::trace("EndScene 初始化完成。");
}

bool EndScene::initUi()
{
    spdlog::trace("创建 EndScene UI...");
    const glm::vec2 windowSize{ m_context.gameState().logicalSize() };

    if (!m_uiManager->init(windowSize)) {
        spdlog::error("EndScene 中初始化 UiManager 失败!");
        return false;
    }

    // --- 创建主标签 ---
    const bool isWin{ m_gameSessionData->isWin() };
    const std::string message{ isWin ? "YOU WIN! CONGRATS!" : "YOU DIED! TRY AGAIN!" };
    const engine::utils::FColor messageColor{
        isWin ? engine::utils::FColor{ .r = 0.0F, .g = 1.0F, .b = 0.0F, .a = 1.0F }
              : engine::utils::FColor{ .r = 1.0F, .g = 0.0F, .b = 0.0F, .a = 1.0F }
    };

    auto mainLabel = std::make_unique<engine::ui::UiLabel>(m_context.textRenderer(),
                                                           message,
                                                           "assets/fonts/VonwaonBitmap-16px.ttf",
                                                           48,
                                                           messageColor);

    // 设置主标签位置，使其居中并靠上
    const glm::vec2 mainLabelSize{ mainLabel->size() };
    const glm::vec2 mainLabelLocalPos{ (windowSize.x - mainLabelSize.x) / 2.0F,
                                       windowSize.y * 0.3F };
    mainLabel->setLocalPosition(mainLabelLocalPos);

    // 将主标签添加到 UI 管理器
    m_uiManager->addElement(std::move(mainLabel));

    // --- 创建得分标签 ---
    const int score{ m_gameSessionData->currentScore() };
    const std::string scoreText{ "Score: " + std::to_string(score) };
    const int scoreFontSize{ 24 };
    const engine::utils::FColor scoreColor{ .r = 1.0F, .g = 1.0F, .b = 1.0F, .a = 1.0F };

    auto scoreLabel = std::make_unique<engine::ui::UiLabel>(m_context.textRenderer(),
                                                            scoreText,
                                                            "assets/fonts/VonwaonBitmap-16px.ttf",
                                                            scoreFontSize,
                                                            scoreColor);

    // 设置得分标签位置，使其 x 方向居中，y 方向在主标签下方 20 像素
    const glm::vec2 scoreLabelSize{ scoreLabel->size() };
    const glm::vec2 scoreLabelLocalPos{ (windowSize.x - scoreLabelSize.x) / 2.0F,
                                        mainLabelLocalPos.y + scoreLabelSize.y + 20.0F };
    scoreLabel->setLocalPosition(scoreLabelLocalPos);
    m_uiManager->addElement(std::move(scoreLabel));

    // --- 创建最高分标签 ---
    const int highestScore{ m_gameSessionData->highestScore() };
    const std::string highestScoreText{ "Highest Score: " + std::to_string(highestScore) };

    auto highestScoreLabel
        = std::make_unique<engine::ui::UiLabel>(m_context.textRenderer(),
                                                highestScoreText,
                                                "assets/fonts/VonwaonBitmap-16px.ttf",
                                                scoreFontSize,
                                                scoreColor);

    // 设置最高分标签位置，使其 x 方向居中，y 方向在得分标签下方 10 像素
    const glm::vec2 highestScoreLabelSize{ highestScoreLabel->size() };
    const glm::vec2 highestScoreLabelLocalPos = { (windowSize.x - highestScoreLabelSize.x) / 2.0F,
                                                  scoreLabelLocalPos.y + scoreLabelSize.y + 10.0F };
    highestScoreLabel->setLocalPosition(highestScoreLabelLocalPos);
    m_uiManager->addElement(std::move(highestScoreLabel));

    // --- 创建 2 个按钮 --- (2个按钮，设定好大小、间距)
    const glm::vec2 buttonSize{ 120.0F, 40.0F };
    const float buttonSpacing{ 20.0F };
    const float totalButtonWidth{ buttonSize.x * 2 + buttonSpacing };

    // 按钮放在右下角，与边缘间隔 30 像素
    float buttonX{ windowSize.x - totalButtonWidth - 30.0F };
    const float buttonY{ windowSize.y - buttonSize.y - 30.0F };

    // Back Button
    auto backButton = std::make_unique<engine::ui::UiButton>(m_context,
                                                             "assets/textures/UI/buttons/Back1.png",
                                                             "assets/textures/UI/buttons/Back2.png",
                                                             "assets/textures/UI/buttons/Back3.png",
                                                             glm::vec2{ buttonX, buttonY },
                                                             buttonSize,
                                                             [this]() { this->back(); });
    m_uiManager->addElement(std::move(backButton));

    // Restart Button
    buttonX += buttonSize.x + buttonSpacing;
    auto restartButton
        = std::make_unique<engine::ui::UiButton>(m_context,
                                                 "assets/textures/UI/buttons/Restart1.png",
                                                 "assets/textures/UI/buttons/Restart2.png",
                                                 "assets/textures/UI/buttons/Restart3.png",
                                                 glm::vec2{ buttonX, buttonY },
                                                 buttonSize,
                                                 [this]() { this->restart(); });
    m_uiManager->addElement(std::move(restartButton));

    spdlog::trace("EndScene UI 创建完成.");
    return true;
}

void EndScene::back()
{
    spdlog::debug("返回按钮被点击。弹出菜单场景和游戏场景，返回标题界面。");
    // 直接替换为TitleScene
    m_sceneManager.requestReplaceScene(
        std::make_unique<TitleScene>(m_context, m_sceneManager, m_gameSessionData));
}

void EndScene::restart()
{
    spdlog::debug("重新开始按钮被点击。弹出菜单场景和游戏场景，重新开始游戏。");
    // 重置游戏数据
    m_gameSessionData->reset();
    // 直接替换为GameScene
    m_sceneManager.requestReplaceScene(
        std::make_unique<GameScene>(m_context, m_sceneManager, m_gameSessionData));
}

} // namespace game::scene
