#pragma once

#include "../render/sprite.h"
#include "../utils/alignment.h"
#include "component_base.h"

#include <SDL3/SDL_rect.h>

#include <optional>
#include <string>

namespace engine::core {
class Context;
}

namespace engine::resource {
class ResourceManager;
}

namespace engine::component {
class TransformComponent;

/**
 * @brief 管理 GameObject 的视觉表示，通过持有一个 Sprite 对象。
 *
 * 协调 Sprite 数据和渲染逻辑，并与 TransformComponent 交互。
 */
class SpriteComponent final : public ComponentBase
{
    friend class engine::object::GameObject; // 友元不能继承，必须每个子类单独添加

public:
    /**
     * @brief 构造函数
     * @param textureId 纹理资源的标识符。
     * @param resourceManager 资源管理器指针。
     * @param alignment 初始对齐方式。
     * @param sourceRect 可选的源矩形。
     * @param isFlipped 初始翻转状态。
     */
    SpriteComponent(const std::string& textureId,
                    engine::resource::ResourceManager& resourceManager,
                    engine::utils::Alignment alignment = engine::utils::Alignment::None,
                    std::optional<SDL_FRect> sourceRect = std::nullopt,
                    bool isFlipped = false);
    ~SpriteComponent() override = default;

    // 禁止拷贝和移动
    SpriteComponent(const SpriteComponent&) = delete;
    SpriteComponent& operator=(const SpriteComponent&) = delete;
    SpriteComponent(SpriteComponent&&) = delete;
    SpriteComponent& operator=(SpriteComponent&&) = delete;

protected:
    // ComponentBase 虚函数覆盖
    void init() override;                                             ///< @brief 初始化函数需要覆盖
    void update(double deltaTime, engine::core::Context&) override {} ///< @brief 更新函数留空
    void render(engine::core::Context& context) override;             ///< @brief 渲染函数需要覆盖

private:
    ///< @brief 保存资源管理器指针，用于获取纹理大小
    engine::resource::ResourceManager* m_resourceManager{ nullptr };
    ///< @brief 缓存 TransformComponent 指针（非必须），用于更新偏移量。
    TransformComponent* m_transformComponent{ nullptr };

    engine::render::Sprite m_sprite;                                        ///< @brief 精灵对象
    engine::utils::Alignment m_alignment{ engine::utils::Alignment::None }; ///< @brief 对齐方式
    bool m_isHidden{ false }; ///< @brief 是否隐藏（不渲染）

};

} // namespace engine::component
