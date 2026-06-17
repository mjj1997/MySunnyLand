#pragma once

#include "player_state_base.h"

namespace game::component::state {

class HurtState final : public PlayerStateBase
{
    friend class game::component::PlayerComponent;

public:
    HurtState(PlayerComponent* playerComponent)
        : PlayerStateBase{ playerComponent }
    {}

protected:
    void enter() override;
    std::unique_ptr<PlayerStateBase> handleInput(engine::core::Context& context) override;
    std::unique_ptr<PlayerStateBase> update(float deltaTime,
                                            engine::core::Context& context) override;
    void exit() override;

private:
    float m_stunnedTimer{ 0.0f }; ///< @brief 硬直计时器，单位为秒
};

} // namespace game::component::state
