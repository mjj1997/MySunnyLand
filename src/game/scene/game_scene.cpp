#include "game_scene.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/core/context.h"
#include "../../engine/object/game_object.h"

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

} // namespace game::scene
