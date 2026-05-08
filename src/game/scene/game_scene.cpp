#include "game_scene.h"
#include "../../engine/component/collider_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/tilelayer_component.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/core/context.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/object/game_object.h"
#include "../../engine/physics/collider.h"
#include "../../engine/physics/physics_engine.h"
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

    // 注册 “main” 层到物理引擎
    auto* layerObjectMain = findGameObjectByName("main");
    if (layerObjectMain != nullptr) {
        auto* tileLayer = layerObjectMain->getComponent<engine::component::TileLayerComponent>();
        if (tileLayer != nullptr) {
            context().physicsEngine().registerCollisionLayer(tileLayer);
            spdlog::info("已注册 “main” 层到物理引擎。");
        }
    }

    // 创建 testObject
    createTestObject();

    SceneBase::init();
    spdlog::trace("GameScene 初始化完成。");
}

void GameScene::update(float deltaTime)
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

    testObject();
    testCollisionPairs();
}

void GameScene::clean()
{
    SceneBase::clean();
}

void GameScene::createTestObject()
{
    spdlog::trace("在 GameScene 中创建 testObject...");

    // 物体1：受重力的箱子（AABB）
    auto testObject = std::make_unique<engine::object::GameObject>("testObject");
    m_testObject = testObject.get();

    // 添加组件
    testObject->addComponent<engine::component::TransformComponent>(glm::vec2{ 100.0f, 100.0f });
    testObject
        ->addComponent<engine::component::SpriteComponent>("assets/textures/Props/big-crate.png",
                                                           context().resourceManager());
    testObject->addComponent<engine::component::PhysicsComponent>(&context().physicsEngine());
    testObject->addComponent<engine::component::ColliderComponent>(
        std::make_unique<engine::physics::AabbCollider>(glm::vec2{ 32.0f, 32.0f }));

    // 将创建好的 GameObject 添加到场景中 （一定要用std::move，否则传递的是左值）
    addGameObject(std::move(testObject));

    spdlog::trace("testObject 创建并添加到 GameScene 中。");

    // 物体2：静止的箱子（Circle）
    auto testObject2 = std::make_unique<engine::object::GameObject>("testObject2");
    testObject2->addComponent<engine::component::TransformComponent>(glm::vec2{ 50.0f, 250.0f });
    testObject2
        ->addComponent<engine::component::SpriteComponent>("assets/textures/Props/big-crate.png",
                                                           context().resourceManager());
    testObject2->addComponent<engine::component::PhysicsComponent>(&context().physicsEngine(),
                                                                   1.0f,
                                                                   false);
    testObject2->addComponent<engine::component::ColliderComponent>(
        std::make_unique<engine::physics::CircleCollider>(16.0f));
    addGameObject(std::move(testObject2));
    spdlog::trace("testObject2 创建并添加到 GameScene 中。");
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

void GameScene::testObject()
{
    if (!m_testObject) {
        return;
    }

    auto& inputManager = context().inputManager();
    auto* physicsComponent = m_testObject->getComponent<engine::component::PhysicsComponent>();
    if (!physicsComponent) {
        return;
    }

    if (inputManager.isActionDown("moveLeft")) {
        m_testObject->getComponent<engine::component::TransformComponent>()->translate(
            glm::vec2{ -2.0f, 0.0f });
    }
    if (inputManager.isActionDown("moveRight")) {
        m_testObject->getComponent<engine::component::TransformComponent>()->translate(
            glm::vec2{ 2.0f, 0.0f });
    }

    if (inputManager.isActionDown("jump")) {
        physicsComponent->setVelocity(glm::vec2{ physicsComponent->velocity().x, -400.0f });
    }
}

void GameScene::testCollisionPairs()
{
    auto& collisionPairs = context().physicsEngine().collisionPairs();
    for (const auto& pair : collisionPairs) {
        spdlog::info("碰撞对: {} - {}", pair.first->name(), pair.second->name());
    }
}

} // namespace game::scene
