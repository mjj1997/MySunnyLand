#pragma once

#include "../render/sprite.h"
#include "component_base.h"

#include <glm/vec2.hpp>
#include <string>

namespace engine::component {
class TransformComponent;

/**
 * @brief 在背景中渲染可滚动纹理的组件，以创建视差效果。
 *
 * 该组件根据相机的位置和滚动因子来移动纹理。
 */
class ParallaxComponent final : public ComponentBase
{
    friend class engine::object::GameObject;

public:
    /**
     * @brief 构造函数
     * @param textureId 背景纹理的资源 ID。
     * @param scrollFactor 控制背景相对于相机移动速度的因子。
     *                      (0, 0) 表示完全静止。
     *                      (1, 1) 表示与相机完全同步移动。
     *                      (0.5, 0.5) 表示以相机一半的速度移动。
     */
    ParallaxComponent(const std::string& textureId,
                      const glm::vec2& scrollFactor,
                      const glm::bvec2& repeat);

    // --- getters & setters ---
    ///< @brief 设置精灵对象
    void setSprite(const engine::render::Sprite& sprite) { m_sprite = sprite; }
    ///< @brief 设置滚动速度因子
    void setScrollFactor(const glm::vec2& factor) { m_scrollFactor = factor; }
    ///< @brief 设置是否重复
    void setRepeat(const glm::bvec2& repeat) { m_repeat = repeat; }
    ///< @brief 设置是否隐藏（不渲染）
    void setHidden(bool hidden) { m_isHidden = hidden; }

    const engine::render::Sprite& sprite() const { return m_sprite; } ///< @brief 获取精灵对象
    const glm::vec2& scrollFactor() const { return m_scrollFactor; }  ///< @brief 获取滚动速度因子
    const glm::bvec2& repeat() const { return m_repeat; }             ///< @brief 获取是否重复
    bool isHidden() const { return m_isHidden; } ///< @brief 获取是否隐藏（不渲染）

protected:
    // 核心循环函数覆盖
    void init() override;
    void update(double deltaTime, engine::core::Context&) override {} // 必须实现纯虚函数，留空
    void render(engine::core::Context& context) override;

private:
    ///< @brief 缓存变换组件
    TransformComponent* m_transformComponent{ nullptr };

    engine::render::Sprite m_sprite; ///< @brief 精灵对象
    glm::vec2 m_scrollFactor;        ///< @brief 滚动速度因子 (0=静止, 1=随相机移动, <1=比相机慢)
    glm::bvec2 m_repeat;             ///< @brief 是否沿着X和Y轴周期性重复
    bool m_isHidden{ false };        ///< @brief 是否隐藏（不渲染）
};

} // namespace engine::component
