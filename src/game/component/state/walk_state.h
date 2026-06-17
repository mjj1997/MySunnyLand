#pragma once

#include "player_state_base.h"

namespace game::component::state {

class WalkState final : public PlayerStateBase
{
    friend class game::component::PlayerComponent;

public:
    explicit WalkState(PlayerComponent* playerComponent)
        : PlayerStateBase{ playerComponent }
    {}

protected:
    void enter() override;
    std::unique_ptr<PlayerStateBase> handleInput(engine::core::Context& context) override;
    std::unique_ptr<PlayerStateBase> update(float deltaTime,
                                            engine::core::Context& context) override;
    void exit() override;
};

} // namespace game::component::state
