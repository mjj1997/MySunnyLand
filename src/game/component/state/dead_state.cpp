#include "dead_state.h"
#include "../player_component.h"

#include "../../../engine/component/audio_component.h"
#include "../../../engine/component/collider_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/object/game_object.h"

#include <spdlog/spdlog.h>

namespace game::component::state {

void DeadState::enter()
{
    spdlog::debug("玩家进入死亡状态。");
    // 播放死亡（受伤）动画
    playAnimation("hurt");

    // 造成向上击退效果
    auto physicsComponent = m_playerComponent->physicsComponent();
    physicsComponent->setVelocity(glm::vec2{ 0.0f, -200.0f });

    // 禁用碰撞检测，形成自动掉出屏幕的效果
    auto colliderComponent = m_playerComponent->owner()
                                 ->getComponent<engine::component::ColliderComponent>();
    if (colliderComponent) {
        colliderComponent->setActive(false);
    }

    if (auto* audioComponent = m_playerComponent->audioComponent(); audioComponent) {
        audioComponent->playSound("dead"); // 播放死亡音效
    }
}

std::unique_ptr<PlayerStateBase> DeadState::handleInput(engine::core::Context& context)
{
    // 死亡状态下不处理输入
    return nullptr;
}

std::unique_ptr<PlayerStateBase> DeadState::update(float deltaTime, engine::core::Context& context)
{
    // 死亡状态下不更新状态
    return nullptr;
}

void DeadState::exit() {}

} // namespace game::component::state
