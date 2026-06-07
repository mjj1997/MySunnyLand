#include "ui_element_base.h"

#include <algorithm>

namespace engine::ui {

UiElementBase::UiElementBase(const glm::vec2& localPosition, const glm::vec2& size)
    : m_localPosition{ localPosition }
    , m_size{ size }
{}

bool UiElementBase::handleInput(engine::core::Context& context)
{
    // 如果元素不可见，直接返回 false
    if (!m_isVisible) {
        return false;
    }

    // 遍历子元素，处理输入。如果子元素应该移除，直接删除。
    for (auto it = m_children.begin(); it != m_children.end();) {
        if (*it && (*it)->shouldRemove() == false) {
            if ((*it)->handleInput(context)) {
                return true;
            }
            ++it;
        } else {
            it = m_children.erase(it);
        }
    }

    // 如果没有子元素处理输入，说明事件没有被处理，返回 false
    return false;
}

void UiElementBase::addChild(std::unique_ptr<UiElementBase> child)
{
    if (child) {
        child->setParent(this); // 设置子元素指向父元素的指针
        m_children.push_back(std::move(child));
    }
}

std::unique_ptr<UiElementBase> UiElementBase::removeChild(UiElementBase* child)
{
    auto it = std::find_if(m_children.begin(), m_children.end(), [child](const auto& p) {
        return p.get() == child;
    });

    if (it != m_children.end()) {
        std::unique_ptr<UiElementBase> removedChild{ std::move(*it) };
        m_children.erase(it);
        removedChild->setParent(nullptr); // 清除父指针
        return removedChild;              // 返回被移除的子元素（可以挂载到别的父 UI 元素下）
    }

    return nullptr;
}

void UiElementBase::removeAllChildren()
{
    for (auto& child : m_children) {
        child->setParent(nullptr); // 清除父指针
    }

    m_children.clear();
}

} // namespace engine::ui
