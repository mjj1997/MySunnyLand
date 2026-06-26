#include "ui_manager.h"
#include "ui_panel.h"

#include <spdlog/spdlog.h>

namespace engine::ui {

UiManager::UiManager() // 创建一个无特定大小和位置的Panel，它的子元素将基于它定位。
    : m_rootElement{ std::make_unique<UiPanel>(glm::vec2(0.0F), glm::vec2(0.0F)) }
{
    spdlog::trace("UI 管理器构造完成。");
}

UiManager::~UiManager() = default;

bool UiManager::init(const glm::vec2& windowSize)
{
    m_rootElement->setSize(windowSize);
    spdlog::trace("UI 管理器已初始化根面板。");
    return true;
}

bool UiManager::handleInput(engine::core::Context& context)
{
    if (m_rootElement && m_rootElement->isVisible()) {
        // 从根元素开始向下分发事件
        if (m_rootElement->handleInput(context)) {
            return true;
        }
    }

    return false;
}

void UiManager::update(float deltaTime, engine::core::Context& context)
{
    if (m_rootElement && m_rootElement->isVisible()) {
        // 从根元素开始向下更新
        m_rootElement->update(deltaTime, context);
    }
}

void UiManager::render(engine::core::Context& context)
{
    if (m_rootElement && m_rootElement->isVisible()) {
        // 从根元素开始向下渲染
        m_rootElement->render(context);
    }
}

void UiManager::addElement(std::unique_ptr<UiElementBase> element)
{
    if (m_rootElement) {
        m_rootElement->addChild(std::move(element));
    } else {
        spdlog::error("无法添加元素到UI管理器，根元素未初始化。");
    }
}

void UiManager::clearElements()
{
    if (m_rootElement) {
        m_rootElement->removeAllChildren();
        spdlog::trace("所有 UI 元素已从 UI 管理器中清除。");
    }
}

} // namespace engine::ui
