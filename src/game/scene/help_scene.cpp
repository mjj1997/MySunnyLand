#include "help_scene.h"

#include "../../engine/core/context.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/scene/scene_manager.h"
#include "../../engine/ui/ui_image.h"
#include "../../engine/ui/ui_manager.h"

#include <spdlog/spdlog.h>

namespace game::scene {

HelpScene::HelpScene(engine::core::Context& context, engine::scene::SceneManager& sceneManager)
    : SceneBase{ "HelpScene", context, sceneManager }
{
    spdlog::trace("HelpScene 构造完成。");
}

void HelpScene::init()
{
    if (m_isInitialized) {
        spdlog::warn("HelpScene 已初始化，重复调用 init()");
        return;
    }

    spdlog::trace("HelpScene 初始化开始...");

    // 创建帮助图片
    const glm::vec2 windowSize{ 640.0F, 360.0F };
    auto helpImage = std::make_unique<engine::ui::UiImage>("assets/textures/UI/instructions.png",
                                                           glm::vec2(0.0F),
                                                           windowSize);
    m_uiManager->addElement(std::move(helpImage));

    SceneBase::init();
    spdlog::trace("HelpScene 初始化完成。");
}

void HelpScene::handleInput()
{
    if (!m_isInitialized) {
        return;
    }

    // 检测是否点击了鼠标左键
    if (m_context.inputManager().isActionPressed("mouseLeftClick")) {
        spdlog::debug("点击鼠标左键，退出帮助场景。");
        m_sceneManager.requestPopScene();
    }
}

} // namespace game::scene
