#pragma once

#include "player_state_base.h"

namespace game::component::state {

class IdleState final : public PlayerStateBase
{
    friend class game::component::PlayerComponent;

public:
    explicit IdleState(PlayerComponent* playerComponent)
        : PlayerStateBase{ playerComponent }
    {}
    ~IdleState() override = default;

protected:
    void enter() override;
    std::unique_ptr<PlayerStateBase> handleInput(engine::core::Context& context) override;
    std::unique_ptr<PlayerStateBase> update(float deltaTime,
                                            engine::core::Context& context) override;
    void exit() override;
};

} // namespace game::component::state
