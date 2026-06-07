#include "ui_element_base.h"

namespace engine::ui {

UiElementBase::UiElementBase(const glm::vec2& localPosition, const glm::vec2& size)
    : m_localPosition{ localPosition }
    , m_size{ size }
{}

void UiElementBase::addChild(std::unique_ptr<UiElementBase> child)
{
    if (child) {
        child->setParent(this); // 设置子元素指向父元素的指针
        m_children.push_back(std::move(child));
    }
}

} // namespace engine::ui
