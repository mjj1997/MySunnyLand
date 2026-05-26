#pragma once

namespace game::component {
class AiComponent;
}

namespace game::component::ai {

/**
 * @brief AI 行为策略的抽象基类。
 */
class AiBehaviorBase
{
    friend class game::component::AiComponent;

public:
    AiBehaviorBase() = default;
    virtual ~AiBehaviorBase() = default;

    //禁止移动和拷贝
    AiBehaviorBase(const AiBehaviorBase&) = delete;
    AiBehaviorBase& operator=(const AiBehaviorBase&) = delete;
    AiBehaviorBase(AiBehaviorBase&&) = delete;
    AiBehaviorBase& operator=(AiBehaviorBase&&) = delete;

protected:
    // --- 没有保存owner指针，因此需要传入 AiComponent 引用 ---
    ///< @brief enter函数可选是否实现，默认为空
    virtual void enter(AiComponent& aiComponent) {}
    ///< @brief 更新 AI 行为逻辑(具体策略)，必须实现
    virtual void update(float deltaTime, AiComponent& aiComponent) = 0;
};

} // namespace game::component::ai
