#include "help_scene.h"

#include <spdlog/spdlog.h>

namespace game::scene {

HelpScene::HelpScene(engine::core::Context& context, engine::scene::SceneManager& sceneManager)
    : SceneBase{ "HelpScene", context, sceneManager }
{
    spdlog::trace("HelpScene 构造完成。");
}

} // namespace game::scene
