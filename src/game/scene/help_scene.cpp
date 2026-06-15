#include "help_scene.h"

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
    glm::vec2 windowSize{ 640.0f, 360.0f };
    auto helpImage = std::make_unique<engine::ui::UiImage>("assets/textures/UI/instructions.png",
                                                           glm::vec2{ 0.0f, 0.0f },
                                                           windowSize);
    m_uiManager->addElement(std::move(helpImage));

    SceneBase::init();
    spdlog::trace("HelpScene 初始化完成。");
}

} // namespace game::scene
