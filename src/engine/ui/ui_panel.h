#pragma once

#include "../utils/math.h"
#include "ui_element_base.h"

#include <optional>

namespace engine::ui {

/**
 * @brief 用于对其他 UI 元素进行分组的容器 UI 元素
 *
 * Panel 通常用于布局和组织。
 * 可以选择是否绘制背景色(纯色)。
 */
class UiPanel final : public UiElementBase
{
public:
    /**
     * @brief 构造一个 Panel
     *
     * @param localPosition Panel 的局部位置  
     * @param size Panel 的大小
     * @param backgroundColor 背景色
     */
    explicit UiPanel(glm::vec2 localPosition = { 0.0F, 0.0F },
                     glm::vec2 size = { 0.0F, 0.0F },
                     std::optional<engine::utils::FColor> backgroundColor = std::nullopt);

    void render(engine::core::Context& context) override;

    void setBackgroundColor(std::optional<engine::utils::FColor> backgroundColor);
    const std::optional<engine::utils::FColor>& backgroundColor() const;

private:
    std::optional<engine::utils::FColor> m_backgroundColor; ///< @brief 可选背景色
};

} // namespace engine::ui
