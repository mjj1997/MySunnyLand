#include "player_state_base.h"
#include "../player_component.h"

#include "../../../engine/component/animation_component.h"
#include "../../../engine/object/game_object.h"

#include <spdlog/spdlog.h>

namespace game::component::state {

void PlayerStateBase::playAnimation(std::string_view animationName)
{
    if (!m_playerComponent) {
        spdlog::error("PlayerState 没有关联的 PlayerComponent，无法播放动画 '{}'", animationName);
        return;
    }

    auto animationComponent = m_playerComponent->animationComponent();
    if (!animationComponent) {
        spdlog::error("PlayerComponent '{}' 没有 AnimationComponent，无法播放动画 '{}'",
                      m_playerComponent->owner()->name(),
                      animationName);
        return;
    }

    animationComponent->playAnimation(animationName);
}

} // namespace game::component::state
