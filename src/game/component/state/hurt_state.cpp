#include "hurt_state.h"

namespace game::component::state {

void HurtState::enter()
{
}

std::unique_ptr<PlayerStateBase> HurtState::handleInput(engine::core::Context& context)
{
    return nullptr;
}

std::unique_ptr<PlayerStateBase> HurtState::update(float deltaTime, engine::core::Context& context)
{
    return nullptr;
}

void HurtState::exit() {}

} // namespace game::component::state
