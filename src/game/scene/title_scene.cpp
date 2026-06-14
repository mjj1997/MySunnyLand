#include "title_scene.h"
#include "../data/session_data.h"

#include "../../engine/audio/audio_player.h"
#include "../../engine/core/context.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/render/camera.h"
#include "../../engine/scene/level_loader.h"

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

    // 加载背景
    engine::scene::LevelLoader levelLoader;
    if (!levelLoader.loadLevel("assets/maps/level0.tmj", *this)) {
        spdlog::error("加载背景失败。");
        return;
    }

    // TODO: 初始化 UI

    // 设置音量
    m_context.audioPlayer().setMusicVolume(0.2f); // 设置背景音乐音量为 20%
    m_context.audioPlayer().setSoundVolume(0.5f); // 设置音效音量为 50%
    // 播放背景音乐
    m_context.audioPlayer().playMusic("assets/audio/platformer_level03_loop.ogg");

    SceneBase::init();
    spdlog::trace("TitleScene 初始化完成。");
}

void TitleScene::update(float deltaTime)
{
    SceneBase::update(deltaTime);

    // 相机自动向右移动
    m_context.camera().move(glm::vec2{ deltaTime * 100.0f, 0.0f });
}

} // namespace game::scene
