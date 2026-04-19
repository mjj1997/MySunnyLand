#pragma once

#include "component_base.h"

#include <glm/vec2.hpp>

namespace engine::component {

/**
 * @class TransformComponent
 * @brief 管理 GameObject 的位置、旋转和缩放。
 */
class TransformComponent final : public ComponentBase
{
    friend class engine::object::GameObject; // 友元不能继承，必须每个子类单独添加

public:
    /**
     * @brief 构造函数
     * @param position 位置
     * @param scale 缩放
     * @param rotation 旋转
     */
    explicit TransformComponent(glm::vec2 position = { 0.0f, 0.0f },
                                glm::vec2 scale = { 1.0f, 1.0f },
                                float rotation = 0.0f)
        : m_position{ position }
        , m_scale{ scale }
        , m_rotation{ rotation }
    {}

    // 禁止拷贝和移动
    TransformComponent(const TransformComponent&) = delete;
    TransformComponent& operator=(const TransformComponent&) = delete;
    TransformComponent(TransformComponent&&) = delete;
    TransformComponent& operator=(TransformComponent&&) = delete;

    void translate(const glm::vec2& offset) { m_position += offset; } ///< @brief 平移

    // Getters and setters
    const glm::vec2& position() const { return m_position; }               ///< @brief 获取位置
    const glm::vec2& scale() const { return m_scale; }                     ///< @brief 获取缩放
    float rotation() const { return m_rotation; }                          ///< @brief 获取旋转
    void setPosition(const glm::vec2& position) { m_position = position; } ///< @brief 设置位置
    void setScale(const glm::vec2& scale); ///< @brief 设置缩放。应用缩放时应同步更新 Sprite 偏移量
    void setRotation(float rotation) { m_rotation = rotation; } ///< @brief 设置旋转

protected:
    ///< @brief 覆盖纯虚函数，这里不需要实现
    void update(double deltaTime, engine::core::Context&) override {}

private:
    glm::vec2 m_position{ 0.0f, 0.0f }; ///< @brief 位置
    glm::vec2 m_scale{ 1.0f, 1.0f };    ///< @brief 缩放
    float m_rotation{ 0.0f };           ///< @brief 角度制，单位：度
};

} // namespace engine::component
