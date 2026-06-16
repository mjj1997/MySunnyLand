#pragma once

#include "../../engine/scene/scene_base.h"

namespace game::scene {

/**
 * @brief 显示帮助信息的场景。
 *
 * 显示一张帮助图片，按鼠标左键退出。
 */
class HelpScene final : public engine::scene::SceneBase
{
public:
    /**
     * @brief HelpScene 的构造函数。
     * @param context 引擎上下文的引用。
     * @param sceneManager 场景管理器的引用。
     */
    HelpScene(engine::core::Context& context, engine::scene::SceneManager& sceneManager);

    // --- 核心方法 ---
    void init() override;
    void handleInput() override;
};

} // namespace game::scene
