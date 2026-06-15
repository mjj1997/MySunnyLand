#pragma once

#include "../../engine/scene/scene_base.h"

namespace game::data {
class SessionData;
}

namespace game::scene {

/**
 * @brief 标题场景类，提供4个按钮：开始游戏、加载游戏、帮助、退出
 */
class TitleScene final : public engine::scene::SceneBase
{
public:
    /**
     * @brief 构造函数
     * @param context 引擎上下文
     * @param sceneManager 场景管理器
     * @param gameSessionData 指向游戏玩法状态的共享指针
     */
    TitleScene(engine::core::Context& context,
               engine::scene::SceneManager& sceneManager,
               std::shared_ptr<game::data::SessionData> gameSessionData = nullptr);

    // --- 核心方法 --- //
    void init() override;
    void update(float deltaTime) override;

private:
    ///< @brief 初始化 UI
    [[nodiscard]] bool initUi();

    // --- 按钮回调函数 ---
    void startGame();
    void loadGame();
    void help();
    void quit();

    ///< @brief 场景间共享数据
    std::shared_ptr<game::data::SessionData> m_gameSessionData{ nullptr };
};

} // namespace game::scene
