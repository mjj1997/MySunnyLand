#include "player_component.h"

#include <spdlog/spdlog.h>

namespace game::component {

void PlayerComponent::setState(std::unique_ptr<state::PlayerStateBase> newState)
{
}

void PlayerComponent::init()
{
}

void PlayerComponent::handleInput(engine::core::Context& context)
{
}

void PlayerComponent::update(float deltaTime, engine::core::Context& context)
{
}

} // namespace game::component
