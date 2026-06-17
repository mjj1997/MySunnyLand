#include "engine/core/game_app.h"
#include "engine/scene/scene_manager.h"
#include "game/scene/title_scene.h"

#include <spdlog/spdlog.h>

void setupInitialScene(engine::scene::SceneManager& sceneManager)
{
    // GameApp在调用run方法之前，先创建并设置初始场景
    auto initialScene = std::make_unique<game::scene::TitleScene>(sceneManager.context(),
                                                                  sceneManager);
    sceneManager.requestPushScene(std::move(initialScene));
}

int main(int, char*[])
{
    spdlog::set_level(spdlog::level::off);

    engine::core::GameApp app;
    app.run();
    return 0;
}
