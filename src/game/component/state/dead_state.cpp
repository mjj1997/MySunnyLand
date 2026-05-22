#include "dead_state.h"

namespace game::component::state {

void DeadState::enter() {}

std::unique_ptr<PlayerStateBase> DeadState::handleInput(engine::core::Context& context)
{
    return std::unique_ptr<PlayerStateBase>();
}

std::unique_ptr<PlayerStateBase> DeadState::update(float deltaTime, engine::core::Context& context)
{
    return std::unique_ptr<PlayerStateBase>();
}

void DeadState::exit() {}

} // namespace game::component::state
