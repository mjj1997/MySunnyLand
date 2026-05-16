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

    // 获取玩家对象
    m_player = findGameObjectByName("player");
    if (!m_player) {
        spdlog::error("未找到玩家对象。");
        return;
    }

    // 设置世界边界
    auto worldSize = layerObjectMain->getComponent<engine::component::TileLayerComponent>()
                         ->worldSize();
    context().physicsEngine().setWorldBounds(
        engine::utils::Rect{ glm::vec2{ 0.0f, 0.0f }, worldSize });

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

    testPlayer();
    testCollisionPairs();
}

void GameScene::clean()
{
    SceneBase::clean();
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

void GameScene::testPlayer()
{
    if (!m_player) {
        return;
    }

    auto& inputManager = context().inputManager();
    auto* physicsComponent = m_player->getComponent<engine::component::PhysicsComponent>();
    if (!physicsComponent) {
        return;
    }

    if (inputManager.isActionDown("moveLeft")) {
        physicsComponent->setVelocity(glm::vec2{ -100.0f, physicsComponent->velocity().y });
    } else {
        physicsComponent->setVelocity(
            glm::vec2{ 0.9f * physicsComponent->velocity().x, physicsComponent->velocity().y });
    }

    if (inputManager.isActionDown("moveRight")) {
        physicsComponent->setVelocity(glm::vec2{ 100.0f, physicsComponent->velocity().y });
    } else {
        physicsComponent->setVelocity(
            glm::vec2{ 0.9f * physicsComponent->velocity().x, physicsComponent->velocity().y });
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
