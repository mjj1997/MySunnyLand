#include "menu_scene.h"
#include "../data/session_data.h"

#include "../../engine/core/context.h"
#include "../../engine/core/game_state.h"

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

    // TODO: 初始化 UI

    SceneBase::init();
    spdlog::trace("MenuScene 初始化完成。");
}

} // namespace game::scene
