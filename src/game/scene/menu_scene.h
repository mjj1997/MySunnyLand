#pragma once

#include "../../engine/scene/scene_base.h"

namespace game::data {
class SessionData;
}

namespace game::scene {

/**
 * @brief 游戏暂停时显示的菜单场景，提供继续、保存、返回、退出等选项。
 * 该场景通常被推送到 GameScene 之上。
 */
class MenuScene final : public engine::scene::SceneBase
{
public:
    /**
     * @brief MenuScene 的构造函数
     * @param context 引擎上下文的引用
     * @param sceneManager 场景管理器的引用
     * @param gameSessionData 场景间传递的游戏数据
     */
    MenuScene(engine::core::Context& context,
              engine::scene::SceneManager& sceneManager,
              std::shared_ptr<game::data::SessionData> gameSessionData = nullptr);

    // --- 核心方法 ---
    void init() override;
    void handleInput() override;

private:
    ///< @brief 初始化 UI
    [[nodiscard]] bool initUi();

    // --- 按钮回调函数 ---
    void resume();
    void save();
    void back();

    ///< @brief 场景间共享数据
    std::shared_ptr<game::data::SessionData> m_gameSessionData{ nullptr };
};

} // namespace game::scene
