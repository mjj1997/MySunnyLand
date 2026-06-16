#include "end_scene.h"
#include "../data/session_data.h"

#include <spdlog/spdlog.h>

namespace game::scene {

EndScene::EndScene(engine::core::Context& context,
                   engine::scene::SceneManager& sceneManager,
                   std::shared_ptr<game::data::SessionData> gameSessionData)
    : SceneBase{ "EndScene", context, sceneManager }
    , m_gameSessionData{ gameSessionData }
{
    if (m_gameSessionData == nullptr) {
        spdlog::error("错误：结束场景收到了空的游戏数据！");
    }
    spdlog::trace("EndScene (胜利：{}) 构造完成。", m_gameSessionData->isWin() ? "是" : "否");
}

} // namespace game::scene
