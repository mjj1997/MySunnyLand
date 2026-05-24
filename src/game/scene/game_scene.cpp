#include "game_scene.h"
#include "../component/player_component.h"

#include "../../engine/component/animation_component.h"
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
    if (m_isInitialized) {
        spdlog::warn("GameScene 已初始化，重复调用 init()");
        return;
    }

    spdlog::trace("GameScene 初始化开始...");

    if (!initLevel()) {
        spdlog::error("初始化关卡失败，无法继续。");
        m_context.inputManager().setShouldQuit(true);
        return;
    }

    if (!initPlayer()) {
        spdlog::error("初始化玩家失败，无法继续。");
        m_context.inputManager().setShouldQuit(true);
        return;
    }

    if (!initEnemyAndItem()) {
        spdlog::error("初始化敌人和物品失败，无法继续。");
        m_context.inputManager().setShouldQuit(true);
        return;
    }

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
}

void GameScene::clean()
{
    SceneBase::clean();
}

bool GameScene::initLevel()
{
    // 加载关卡（level_loader通常加载完成后即可销毁，因此不存为成员变量）
    engine::scene::LevelLoader levelLoader;
    if (!levelLoader.loadLevel("assets/maps/level1.tmj", *this)) {
        spdlog::error("加载关卡失败。");
        return false;
    }

    // 注册 “main” 层到物理引擎
    auto* layerObjectMain = findGameObjectByName("main");
    if (layerObjectMain == nullptr) {
        spdlog::error("未找到 “main” 层。");
        return false;
    }

    auto* tileLayer = layerObjectMain->getComponent<engine::component::TileLayerComponent>();
    if (tileLayer == nullptr) {
        spdlog::error("未找到 “main” 层的 TileLayerComponent。");
        return false;
    }
    m_context.physicsEngine().registerCollisionLayer(tileLayer);
    spdlog::info("已注册 “main” 层到物理引擎。");

    // 设置世界边界
    auto worldSize = layerObjectMain->getComponent<engine::component::TileLayerComponent>()
                         ->worldSize();
    m_context.physicsEngine().setWorldBounds(
        engine::utils::Rect{ glm::vec2{ 0.0f, 0.0f }, worldSize });

    // 设置相机边界
    m_context.camera().setLimitBounds(engine::utils::Rect{ glm::vec2{ 0.0f, 0.0f }, worldSize });

    spdlog::trace("关卡初始化完成。");
    return true;
}

bool GameScene::initPlayer()
{
    // 获取玩家对象
    m_player = findGameObjectByName("player");
    if (!m_player) {
        spdlog::error("未找到玩家对象。");
        return false;
    }

    // 添加 PlayerComponent 到玩家对象
    auto* playerComponent = m_player->addComponent<game::component::PlayerComponent>();
    if (playerComponent == nullptr) {
        spdlog::error("无法添加 PlayerComponent 到玩家对象。");
        return false;
    }

    // 设置相机跟随玩家
    auto playerTransformComponent = m_player->getComponent<engine::component::TransformComponent>();
    if (playerTransformComponent == nullptr) {
        spdlog::error("玩家对象缺少 TransformComponent，无法设置相机目标。");
        return false;
    }
    m_context.camera().setTarget(playerTransformComponent);

    spdlog::trace("Player 初始化完成。");
    return true;
}

bool GameScene::initEnemyAndItem()
{
    bool success{ true };
    for (auto& gameObject : m_gameObjects) {
        if (gameObject->name() == "eagle") {
            if (auto* animationComponent
                = gameObject->getComponent<engine::component::AnimationComponent>();
                animationComponent) {
                animationComponent->playAnimation("fly");
            } else {
                spdlog::error("Eagle 对象缺少 AnimationComponent，无法播放动画。");
                success = false;
            }
        }
        if (gameObject->name() == "frog") {
            if (auto* animationComponent
                = gameObject->getComponent<engine::component::AnimationComponent>();
                animationComponent) {
                animationComponent->playAnimation("idle");
            } else {
                spdlog::error("Frog 对象缺少 AnimationComponent，无法播放动画。");
                success = false;
            }
        }
        if (gameObject->name() == "opossum") {
            if (auto* animationComponent
                = gameObject->getComponent<engine::component::AnimationComponent>();
                animationComponent) {
                animationComponent->playAnimation("walk");
            } else {
                spdlog::error("Opossum 对象缺少 AnimationComponent，无法播放动画。");
                success = false;
            }
        }
        if (gameObject->tag() == "item") {
            if (auto* animationComponent
                = gameObject->getComponent<engine::component::AnimationComponent>();
                animationComponent) {
                animationComponent->playAnimation("idle");
            } else {
                spdlog::error("Item 对象缺少 AnimationComponent，无法播放动画。");
                success = false;
            }
        }
    }

    return success;
}

void GameScene::handleObjectCollisions()
{
    // 从物理引擎获取碰撞对列表
    auto collisionPairs = m_context.physicsEngine().collisionPairs();
    // 遍历碰撞对列表
    for (auto& pair : collisionPairs) {
        auto* obj1 = pair.first;
        auto* obj2 = pair.second;

        // 处理玩家与敌人的碰撞
        if (obj1->name() == "player" && obj2->tag() == "enemy") {
            handlePlayerVsEnemyCollision(obj1, obj2);
        } else if (obj2->name() == "player" && obj1->tag() == "enemy") {
            handlePlayerVsEnemyCollision(obj2, obj1);
        }
        // 处理玩家与道具的碰撞
        else if (obj1->name() == "player" && obj2->tag() == "item") {
            handlePlayerVsItemCollision(obj1, obj2);
        } else if (obj2->name() == "player" && obj1->tag() == "item") {
            handlePlayerVsItemCollision(obj2, obj1);
        }
    }
}

void GameScene::handlePlayerVsEnemyCollision(engine::object::GameObject* player,
                                             engine::object::GameObject* enemy)
{
}

void GameScene::handlePlayerVsItemCollision(engine::object::GameObject* player,
                                            engine::object::GameObject* item)
{
}

} // namespace game::scene
