#include "game_scene.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/core/context.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/object/game_object.h"
#include "../../engine/render/camera.h"
#include "../../engine/scene/level_loader.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace game::scene {

// 构造函数: 调用基类构造函数
GameScene::GameScene(std::string name,
                     engine::core::Context& context,
                     engine::scene::SceneManager& sceneManager)
    : SceneBase{ name, context, sceneManager }
{
    spdlog::trace("GameScene 构造完成。");
}

void GameScene::init()
{
    // 加载关卡
    engine::scene::LevelLoader levelLoader;
    levelLoader.loadLevel("assets/maps/level1.tmj", *this);

    // 创建 testObject
    createTestObject();

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

    testCamera();
}

void GameScene::clean()
{
    SceneBase::clean();
}

void GameScene::createTestObject()
{
    spdlog::trace("在 GameScene 中创建 testObject...");

    auto testObject = std::make_unique<engine::object::GameObject>("testObject");

    // 添加组件
    testObject->addComponent<engine::component::TransformComponent>(glm::vec2{ 100.0f, 100.0f });
    testObject
        ->addComponent<engine::component::SpriteComponent>("assets/textures/Props/big-crate.png",
                                                           context().resourceManager());

    // 将创建好的 GameObject 添加到场景中 （一定要用std::move，否则传递的是左值）
    addGameObject(std::move(testObject));

    spdlog::trace("testObject 创建并添加到 GameScene 中。");
}

void GameScene::testCamera()
{
    auto& camera = m_context.camera();
    auto& inputManager = m_context.inputManager();
    if (inputManager.isActionDown("moveUp")) {
        camera.move(glm::vec2{ 0.0f, -2.0f });
    }
    if (inputManager.isActionDown("moveDown")) {
        camera.move(glm::vec2{ 0.0f, 2.0f });
    }
    if (inputManager.isActionDown("moveLeft")) {
        camera.move(glm::vec2{ -2.0f, 0.0f });
    }
    if (inputManager.isActionDown("moveRight")) {
        camera.move(glm::vec2{ 2.0f, 0.0f });
    }
}

} // namespace game::scene
