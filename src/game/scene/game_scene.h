#pragma once

#include "../../engine/scene/scene_base.h"

#include <glm/vec2.hpp>

namespace game::data {
class SessionData;
}

namespace engine::ui {
class UiLabel;
class UiPanel;
} // namespace engine::ui

namespace game::scene {

/**
 * @brief 主要的游戏场景，包含玩家、敌人、关卡元素等。
 */
class GameScene final : public engine::scene::SceneBase
{
public:
    /**
     * @brief 构造函数
     * @param context 引擎上下文
     * @param sceneManager 场景管理器
     * @param gameSessionData 指向游戏玩法状态的共享指针
     */
    GameScene(engine::core::Context& context,
              engine::scene::SceneManager& sceneManager,
              std::shared_ptr<game::data::SessionData> gameSessionData = nullptr);

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
    ///< @brief 初始化 UI
    [[nodiscard]] bool initUi();

    ///< @brief 总入口：处理游戏对象间的碰撞逻辑（从PhysicsEngine获取信息，分发给具体处理函数处理）
    void handleObjectCollisions();
    ///< @brief 分支：处理玩家与敌人之间的碰撞
    void handlePlayerVsEnemyCollision(engine::object::GameObject* player,
                                      engine::object::GameObject* enemy);
    ///< @brief 分支：处理玩家与道具之间的碰撞
    void handlePlayerVsItemCollision(engine::object::GameObject* player,
                                     engine::object::GameObject* item);
    ///< @brief 处理玩家受伤（更新得分、UI等）
    void handlePlayerDamage(int damage);

    ///< @brief 处理瓦片触发事件
    void handleTileTriggers();

    ///< @brief 根据关卡名称获取对应的地图文件路径
    std::string levelNameToPath(const std::string& levelName) const
    {
        return "assets/maps/" + levelName + ".tmj";
    }

    ///< @brief 进入下一个关卡
    void goToNextLevel(engine::object::GameObject* trigger);

    /**
     * @brief 创建一个特效对象（一次性）。
     * @param center 特效中心位置
     * @param tag 特效标签（决定特效类型,例如"enemy","item"）
     */
    void createEffect(const glm::vec2& center, const std::string& tag);

    // --- UI 相关函数 ---
    void createScoreUi();           ///< @brief 创建得分UI
    void createHealthUi();          ///< @brief 创建生命值UI (或最大生命值改变时重设)
    void addScoreWithUi(int score); ///< @brief 增加得分，同时更新UI
    void healWithUi(int amount);    ///< @brief 增加生命，同时更新UI
    void updateHealthWithUi();      ///< @brief 更新生命值UI (只适用最大生命值不变的情况)

    ///< @brief 场景间共享数据
    std::shared_ptr<game::data::SessionData> m_gameSessionData{ nullptr };
    /// @brief 保存玩家对象指针
    engine::object::GameObject* m_player{ nullptr };

    ///< @brief 得分标签（生命周期由 UiManager 管理，因此使用裸指针）
    engine::ui::UiLabel* m_scoreLabel{ nullptr };
    engine::ui::UiPanel* m_healthPanel{ nullptr }; ///< @brief 生命值图标面板
};

} // namespace game::scene
