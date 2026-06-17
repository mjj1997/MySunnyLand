#include "game_scene.h"
#include "../component/ai/jump_behavior.h"
#include "../component/ai/patrol_behavior.h"
#include "../component/ai/updown_behavior.h"
#include "../component/ai_component.h"
#include "../component/player_component.h"
#include "../data/session_data.h"
#include "end_scene.h"
#include "menu_scene.h"

#include "../../engine/audio/audio_player.h"
#include "../../engine/component/animation_component.h"
#include "../../engine/component/collider_component.h"
#include "../../engine/component/health_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/tilelayer_component.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/core/context.h"
#include "../../engine/core/game_state.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/object/game_object.h"
#include "../../engine/physics/collider.h"
#include "../../engine/physics/physics_engine.h"
#include "../../engine/render/animation.h"
#include "../../engine/render/camera.h"
#include "../../engine/render/text_renderer.h"
#include "../../engine/scene/level_loader.h"
#include "../../engine/scene/scene_manager.h"
#include "../../engine/ui/ui_button.h"
#include "../../engine/ui/ui_image.h"
#include "../../engine/ui/ui_label.h"
#include "../../engine/ui/ui_manager.h"
#include "../../engine/ui/ui_panel.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace game::scene {

// 构造函数: 调用基类构造函数
GameScene::GameScene(engine::core::Context& context,
                     engine::scene::SceneManager& sceneManager,
                     std::shared_ptr<game::data::SessionData> gameSessionData)
    : SceneBase{ "GameScene", context, sceneManager }
    , m_gameSessionData{ std::move(gameSessionData) }
{
    if (m_gameSessionData == nullptr) {
        // 如果没有传入 SessionData，创建一个默认的
        m_gameSessionData = std::make_shared<game::data::SessionData>();
        spdlog::info("未提供 SessionData，使用默认值。");
    }
    spdlog::trace("GameScene 构造完成。");
}

void GameScene::init()
{
    if (m_isInitialized) {
        spdlog::warn("GameScene 已初始化，重复调用 init()");
        return;
    }

    spdlog::trace("GameScene 初始化开始...");

    m_context.gameState().setCurrentState(engine::core::State::Playing);
    // 同步最高分
    m_gameSessionData->syncHighestScore("assets/save.json");

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

    if (!initUi()) {
        spdlog::error("初始化 UI 失败，无法继续。");
        m_context.inputManager().setShouldQuit(true);
        return;
    }

    // 播放背景音乐（循环播放、淡入时间 1 秒）
    m_context.audioPlayer().playMusic("assets/audio/hurry_up_and_run.ogg", -1, 1000);

    SceneBase::init();
    spdlog::trace("GameScene 初始化完成。");
}

void GameScene::update(float deltaTime)
{
    SceneBase::update(deltaTime);
    handleObjectCollisions();
    handleTileTriggers();

    // 玩家掉出地图下方则判断为失败
    if (m_player != nullptr) {
        auto pos = m_player->getComponent<engine::component::TransformComponent>()->position();
        auto worldBoundary = m_context.physicsEngine().worldBounds();
        // 多 100 像素冗余量
        if (worldBoundary && pos.y > worldBoundary->position.y + worldBoundary->size.y + 100.0f) {
            spdlog::debug("玩家掉出地图下方，游戏失败");
            showEndScene(false);
        }
    }
}

void GameScene::render()
{
    SceneBase::render();
}

void GameScene::handleInput()
{
    SceneBase::handleInput();

    // 检查暂停键
    if (m_context.inputManager().isActionPressed("pause")) {
        spdlog::debug("在 GameScene 中检测到暂停动作，正在推送 MenuScene。");
        m_sceneManager.requestPushScene(
            std::make_unique<MenuScene>(m_context, m_sceneManager, m_gameSessionData));
    }
}

void GameScene::clean()
{
    SceneBase::clean();
}

bool GameScene::initLevel()
{
    // 加载关卡（level_loader通常加载完成后即可销毁，因此不存为成员变量）
    engine::scene::LevelLoader levelLoader;
    const std::string& mapPath{ m_gameSessionData->mapPath() };
    if (!levelLoader.loadLevel(mapPath, *this)) {
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
    // 开始时重置相机位置，以免切换场景时晃动
    m_context.camera().setPosition(glm::vec2{ 0.0f, 0.0f });

    spdlog::trace("关卡初始化完成。");
    return true;
}

bool GameScene::initPlayer()
{
    // 获取玩家对象
    m_player = findGameObjectByName("player");
    if (m_player == nullptr) {
        spdlog::error("未找到玩家对象。");
        return false;
    }

    // 添加 PlayerComponent 到玩家对象
    if (auto* playerComponent = m_player->addComponent<game::component::PlayerComponent>();
        playerComponent == nullptr) {
        spdlog::error("无法添加 PlayerComponent 到玩家对象。");
        return false;
    }

    // 从 SessionData 中更新玩家的生命值
    if (auto* healthComponent = m_player->getComponent<engine::component::HealthComponent>();
        healthComponent != nullptr) {
        healthComponent->setMaxHealth(m_gameSessionData->maxHealth());
        healthComponent->setCurrentHealth(m_gameSessionData->currentHealth());
    } else {
        spdlog::error("玩家对象缺少 HealthComponent，无法更新生命值。");
        return false;
    }

    // 设置相机跟随玩家
    if (auto* transformComponent = m_player->getComponent<engine::component::TransformComponent>();
        transformComponent != nullptr) {
        m_context.camera().setTarget(transformComponent);
    } else {
        spdlog::error("玩家对象缺少 TransformComponent，无法设置相机目标。");
        return false;
    }

    spdlog::trace("Player 初始化完成。");
    return true;
}

bool GameScene::initEnemyAndItem()
{
    bool success{ true };
    for (auto& gameObject : m_gameObjects) {
        if (gameObject->name() == "eagle") {
            if (auto* aiComponent = gameObject->addComponent<game::component::AiComponent>();
                aiComponent) {
                auto maxY = gameObject->getComponent<engine::component::TransformComponent>()
                                ->position()
                                .y;
                auto minY = maxY - 80.0f; // 鹰的飞行范围（当前位置 ~ 上方 80px 的区域）
                aiComponent->setBehavior(
                    std::make_unique<game::component::ai::UpDownBehavior>(minY, maxY));
            }
        }
        if (gameObject->name() == "frog") {
            if (auto* aiComponent = gameObject->addComponent<game::component::AiComponent>();
                aiComponent) {
                auto maxX
                    = gameObject->getComponent<engine::component::TransformComponent>()->position().x
                      - 10.0f;            // 这里减去 10px 是为了增加青蛙跳跃的稳定性
                auto minX = maxX - 90.0f; // 青蛙的跳跃范围（当前位置 ~ 左方 90px 的区域）
                aiComponent->setBehavior(
                    std::make_unique<game::component::ai::JumpBehavior>(minX, maxX));
            }
        }
        if (gameObject->name() == "opossum") {
            if (auto* aiComponent = gameObject->addComponent<game::component::AiComponent>();
                aiComponent) {
                auto maxX = gameObject->getComponent<engine::component::TransformComponent>()
                                ->position()
                                .x;
                auto minX = maxX - 200.0f;
                aiComponent->setBehavior(
                    std::make_unique<game::component::ai::PatrolBehavior>(minX, maxX));
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

bool GameScene::initUi()
{
    if (!m_uiManager->init(m_context.gameState().logicalSize())) {
        spdlog::error("GameScene 中初始化 UiManager 失败!");
        return false;
    }

    createScoreUi();
    createHealthUi();

    return true;
}

void GameScene::handleObjectCollisions()
{
    // 从物理引擎获取碰撞对列表
    auto collisionPairs = m_context.physicsEngine().collisionPairs();
    // 遍历碰撞对列表
    for (const auto& pair : collisionPairs) {
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
        // 处理玩家与标签为“hazard”的对象的碰撞
        else if (obj1->name() == "player" && obj2->tag() == "hazard") {
            handlePlayerDamage(1);
            spdlog::debug("玩家 {} 收到了 Hazard 对象伤害", obj1->name());
        } else if (obj2->name() == "player" && obj1->tag() == "hazard") {
            handlePlayerDamage(1);
            spdlog::debug("玩家 {} 收到了 Hazard 对象伤害", obj2->name());
        }
        // 处理玩家与标签为“nextLevel”的关底触发器对象的碰撞
        else if (obj1->name() == "player" && obj2->tag() == "nextLevel") {
            goToNextLevel(obj2);
        } else if (obj2->name() == "player" && obj1->tag() == "nextLevel") {
            goToNextLevel(obj1);
        }
        // 处理玩家与名称为“win”的游戏结束触发器对象的碰撞
        else if (obj1->name() == "player" && obj2->name() == "win") {
            showEndScene(true);
        } else if (obj2->name() == "player" && obj1->name() == "win") {
            showEndScene(true);
        }
    }
}

void GameScene::handlePlayerVsEnemyCollision(engine::object::GameObject* player,
                                             engine::object::GameObject* enemy)
{
    /** --- 踩踏判断逻辑 ---
     * 1. 玩家中心点在敌人上方
     * 2. 重叠区域：overlap.x > overlap.y
     */
    auto playerAabb = player->getComponent<engine::component::ColliderComponent>()->worldAabb();
    auto enemyAabb = enemy->getComponent<engine::component::ColliderComponent>()->worldAabb();
    auto playerCenter = playerAabb.position + playerAabb.size / 2.0f;
    auto enemyCenter = enemyAabb.position + enemyAabb.size / 2.0f;
    auto overlap = glm::vec2{ playerAabb.size / 2.0f + enemyAabb.size / 2.0f }
                   - glm::abs(playerCenter - enemyCenter);

    // 踩踏判断成功，敌人受伤
    if (overlap.x > overlap.y && playerCenter.y < enemyCenter.y) {
        spdlog::info("玩家 {} 踩踏了敌人 {}", player->name(), enemy->name());
        // 处理敌人受伤逻辑
        auto enemyHealth = enemy->getComponent<engine::component::HealthComponent>();
        if (!enemyHealth) {
            spdlog::error("敌人 {} 没有 HealthComponent 组件，无法处理踩踏伤害", enemy->name());
            return;
        }
        enemyHealth->takeDamage(1); // 造成1点伤害

        // 播放敌人死亡效果
        if (!enemyHealth->isAlive()) {
            spdlog::info("敌人 {} 被踩踏后死亡", enemy->name());
            enemy->setShouldRemove(true);            // 标记敌人为待删除状态
            createEffect(enemyCenter, enemy->tag()); // 创建（死亡）特效
        }

        // 播放玩家跳起效果
        auto playerPhysicsComponent = player->getComponent<engine::component::PhysicsComponent>();
        playerPhysicsComponent->setVelocity(
            glm::vec2{ playerPhysicsComponent->velocity().x, -300.0f }); // 向上跳起
        // 播放玩家跳起音效（此音效完全可以放在玩家的音频组件中，这里示例另一种用法：直接用 AudioPlayer 播放，传入文件路径）
        m_context.audioPlayer().playSound("assets/audio/punch2a.mp3");
        // 加分
        addScoreWithUi(10);
    }
    // 踩踏判断失败，玩家受伤
    else {
        spdlog::info("敌人 {} 对玩家 {} 造成伤害", enemy->name(), player->name());
        // 处理玩家受伤逻辑
        handlePlayerDamage(1);
    }
}

void GameScene::handlePlayerVsItemCollision(engine::object::GameObject* player,
                                            engine::object::GameObject* item)
{
    if (item->name() == "fruit") {
        healWithUi(1); // 加血
    } else if (item->name() == "gem") {
        // 加分
        addScoreWithUi(5);
    }
    item->setShouldRemove(true); // 标记道具为待删除状态

    // 播放道具反馈特效
    auto itemAabb = item->getComponent<engine::component::ColliderComponent>()->worldAabb();
    auto itemCenter = itemAabb.position + itemAabb.size / 2.0f;
    createEffect(itemCenter, item->tag()); // 创建特效
    // 播放道具反馈音效（此音效完全可以放在道具的音频组件中，这里示例另一种用法：直接用 AudioPlayer 播放，传入文件路径）
    m_context.audioPlayer().playSound("assets/audio/poka01.mp3");
}

void GameScene::handlePlayerDamage(int damage)
{
    auto playerComponent = m_player->getComponent<game::component::PlayerComponent>();
    if (playerComponent->takeDamage(damage) == false) {
        // 没有受伤，直接返回
        return;
    }

    if (playerComponent->isAlive() == false) {
        spdlog::info("玩家 '{}' 死亡", m_player->name());
        // TODO: 可能的死亡逻辑处理
    }

    // 更新生命值以及生命值 UI
    updateHealthWithUi();
}

void GameScene::handleTileTriggers()
{
    const auto& tileTriggersEvents = m_context.physicsEngine().tileTriggerEvents();
    for (const auto& event : tileTriggersEvents) {
        if (auto tileType = event.second; tileType == engine::component::TileType::Hazard) {
            auto* obj = event.first;
            // 玩家与危险瓦片碰撞，玩家受伤
            if (obj->name() == "player") {
                handlePlayerDamage(1);
                spdlog::debug("玩家 {} 收到了 Hazard 对象伤害", obj->name());
            }
            // TODO: 其他对象与危险瓦片碰撞的处理，目前让敌人无视危险瓦片
        }
    }
}

void GameScene::goToNextLevel(const engine::object::GameObject* trigger)
{
    auto sceneName = trigger->name();
    auto mapPath = levelNameToPath(sceneName);
    m_gameSessionData->setNextLevel(mapPath);

    auto nextScene = std::make_unique<game::scene::GameScene>(m_context,
                                                              m_sceneManager,
                                                              m_gameSessionData);
    m_sceneManager.requestReplaceScene(std::move(nextScene));
}

void GameScene::showEndScene(bool isWin)
{
    spdlog::debug("显示结束场景，游戏 {}", isWin ? "胜利" : "失败");
    m_gameSessionData->setIsWin(isWin);
    auto endScene = std::make_unique<game::scene::EndScene>(m_context,
                                                            m_sceneManager,
                                                            m_gameSessionData);
    m_sceneManager.requestPushScene(std::move(endScene));
}

void GameScene::createEffect(glm::vec2 center, const std::string& tag)
{
    // --- 创建游戏对象和变换组件 ---
    auto effectObj = std::make_unique<engine::object::GameObject>("effect_" + tag);
    effectObj->addComponent<engine::component::TransformComponent>(std::move(center));

    // --- 根据标签创建不同的精灵组件和动画---
    auto animation = std::make_unique<engine::render::Animation>("effect", false);
    SDL_FRect srcRect{};
    float duration{ 0.1f };

    if (tag == "enemy") {
        effectObj->addComponent<engine::component::SpriteComponent>(
            "assets/textures/FX/enemy-deadth.png",
            m_context.resourceManager(),
            engine::utils::Alignment::Center);

        for (int i{ 0 }; i < 5; ++i) {
            srcRect = SDL_FRect{ static_cast<float>(i * 40), 0.0f, 40.0f, 41.0f };
            animation->addFrame(srcRect, duration);
        }
    } else if (tag == "item") {
        effectObj->addComponent<engine::component::SpriteComponent>(
            "assets/textures/FX/item-feedback.png",
            m_context.resourceManager(),
            engine::utils::Alignment::Center);

        for (int i{ 0 }; i < 4; ++i) {
            srcRect = SDL_FRect{ static_cast<float>(i * 32), 0.0f, 32.0f, 32.0f };
            animation->addFrame(srcRect, duration);
        }
    } else {
        spdlog::warn("未知特效类型: {}", tag);
        return;
    }

    // --- 根据创建的动画，添加动画组件，并设置为单次播放 ---
    auto* animationComponent = effectObj->addComponent<engine::component::AnimationComponent>();
    animationComponent->addAnimation(std::move(animation));
    animationComponent->setOneShotRemoval(true);
    animationComponent->playAnimation("effect");

    safeAddGameObject(std::move(effectObj)); // 安全添加特效对象
    spdlog::debug("创建特效: {}", tag);
}

void GameScene::createScoreUi()
{
    // 创建得分标签
    const std::string& scoreText{ "Score: " + std::to_string(m_gameSessionData->currentScore()) };
    auto scoreLabel = std::make_unique<engine::ui::UiLabel>(m_context.textRenderer(),
                                                            scoreText,
                                                            "assets/fonts/VonwaonBitmap-16px.ttf",
                                                            16);
    m_scoreLabel = scoreLabel.get(); // 保存指针，方便后续更新

    // 设置得分标签位置，确保在屏幕顶部右侧
    auto screenSize = m_uiManager->rootElement()->size();
    scoreLabel->setLocalPosition(glm::vec2{ screenSize.x - 100.0f, 10.0f });

    // 将得分标签添加到 UI 管理器
    m_uiManager->addElement(std::move(scoreLabel));
}

void GameScene::createHealthUi()
{
    // 创建一个默认的 UiPanel (不需要背景色，因此大小无所谓，只用于定位)
    auto healthPanel = std::make_unique<engine::ui::UiPanel>();
    m_healthPanel = healthPanel.get(); // 保存指针，方便后续更新

    // --- 根据最大生命值，循环创建生命值图标(添加到 UiPanel 中) ---
    glm::vec2 iconStartPos{ 10.0f, 10.0f };
    glm::vec2 iconSize{ 20.0f, 18.0f };
    float spacing{ 5.0f };
    for (int i{ 0 }; i < m_gameSessionData->maxHealth(); ++i) {
        glm::vec2 iconPos{ iconStartPos.x + i * (iconSize.x + spacing), iconStartPos.y };
        // 创建背景图标
        auto bgIcon = std::make_unique<engine::ui::UiImage>("assets/textures/UI/Heart-bg.png",
                                                            iconPos,
                                                            iconSize);
        // 添加背景图标到生命值面板
        healthPanel->addChild(std::move(bgIcon));

        // 创建前景图标
        auto fgIcon = std::make_unique<engine::ui::UiImage>("assets/textures/UI/Heart.png",
                                                            iconPos,
                                                            iconSize);
        // 如果当前生命值不足，设置前景图标不可见
        fgIcon->setVisible(i < m_gameSessionData->currentHealth());

        // 添加前景图标到生命值面板
        healthPanel->addChild(std::move(fgIcon));
    }

    // 将生命值面板添加到 UI 管理器
    m_uiManager->addElement(std::move(healthPanel));
}

void GameScene::addScoreWithUi(int score)
{
    m_gameSessionData->addScore(score);
    const std::string& scoreText{ "Score: " + std::to_string(m_gameSessionData->currentScore()) };
    m_scoreLabel->setText(scoreText);
    spdlog::info("更新得分标签: {}", scoreText);
}

void GameScene::healWithUi(int amount)
{
    m_player->getComponent<engine::component::HealthComponent>()->heal(amount);
    updateHealthWithUi();
}

void GameScene::updateHealthWithUi()
{
    if (!m_player || !m_healthPanel) {
        spdlog::error("玩家对象或生命值面板不存在，无法更新生命值 UI");
        return;
    }

    // 获取当前生命值并更新游戏数据
    int currentHealth{
        m_player->getComponent<engine::component::HealthComponent>()->currentHealth()
    };
    m_gameSessionData->setCurrentHealth(currentHealth);
    int maxHealth{ m_gameSessionData->maxHealth() };

    /** 更新生命值图标可见性
     *  前景图标在奇数索引位置（1, 3, 5, ...），需要根据当前生命值设置可见性
     */
    for (int i{ 0 }; i < maxHealth; ++i) {
        // 前景图标的索引是 i * 2 + 1（奇数位置）
        m_healthPanel->children().at(i * 2 + 1)->setVisible(i < currentHealth);
    }
}

} // namespace game::scene
