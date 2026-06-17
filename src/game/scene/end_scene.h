#pragma once

#include "../../engine/scene/scene_base.h"

namespace game::data {
class SessionData;
}

namespace game::scene {

/**
 * @brief 显示游戏结束（胜利或失败）信息的叠加场景。
 *
 * 提供重新开始或返回主菜单的选项。
 */
class EndScene final : public engine::scene::SceneBase
{
public:
    /**
     * @brief EndScene 的构造函数
     * @param context 引擎上下文的引用
     * @param sceneManager 场景管理器的引用
     * @param gameSessionData 场景间传递的游戏数据
     */
    EndScene(engine::core::Context& context,
             engine::scene::SceneManager& sceneManager,
             std::shared_ptr<game::data::SessionData> gameSessionData = nullptr);

    // --- 核心方法 ---
    void init() override;

private:
    ///< @brief 初始化 UI
    [[nodiscard]] bool initUi();

    // --- 按钮回调函数 ---
    void back();
    void restart();

    ///< @brief 场景间共享数据
    std::shared_ptr<game::data::SessionData> m_gameSessionData{ nullptr };
};

} // namespace game::scene
