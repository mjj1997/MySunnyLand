#include "game_scene.h"

#include <spdlog/spdlog.h>

namespace game::scene {

// 构造函数: 调用基类构造函数
GameScene::GameScene(std::string name, engine::core::Context& context)
    : SceneBase{ name, context }
{
    spdlog::trace("GameScene 构造完成。");
}

void GameScene::init()
{
    SceneBase::init();
    spdlog::trace("GameScene 初始化完成。");
}

void GameScene::update(double deltaTime)
{
    SceneBase::update(deltaTime);
}

void GameScene::render()
{
    SceneBase::render();
}

void GameScene::handleInput()
{
    SceneBase::handleInput();
}

void GameScene::clean()
{
    SceneBase::clean();
}

} // namespace game::scene
