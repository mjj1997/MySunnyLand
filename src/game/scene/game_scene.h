#pragma once

#include "../../engine/scene/scene_base.h"

namespace game::scene {

/**
 * @brief 主要的游戏场景，包含玩家、敌人、关卡元素等。
 */
class GameScene final : public engine::scene::SceneBase
{
public:
    GameScene(std::string name,
              engine::core::Context& context,
              engine::scene::SceneManager& sceneManager);

    // 覆盖场景基类的核心方法
    void init() override;
    void update(float deltaTime) override;
    void render() override;
    void handleInput() override;
    void clean() override;

private:
    ///< @brief 初始化关卡
    [[nodiscard]] bool initLevel();
    ///< @brief 初始化玩家
    [[nodiscard]] bool initPlayer();
    ///< @brief 初始化敌人和道具
    [[nodiscard]] bool initEnemyAndItem();

    ///< @brief 总入口：处理游戏对象间的碰撞逻辑（从PhysicsEngine获取信息，分发给具体处理函数处理）
    void handleObjectCollisions();
    ///< @brief 分支：处理玩家与敌人之间的碰撞
    void handlePlayerVsEnemyCollision(engine::object::GameObject* player,
                                      engine::object::GameObject* enemy);
    ///< @brief 分支：处理玩家与道具之间的碰撞
    void handlePlayerVsItemCollision(engine::object::GameObject* player,
                                     engine::object::GameObject* item);

    /// @brief 保存玩家对象指针
    engine::object::GameObject* m_player{ nullptr };
};

} // namespace game::scene
