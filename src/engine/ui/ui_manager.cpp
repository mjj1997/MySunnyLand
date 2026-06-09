#include "ui_manager.h"
#include "ui_panel.h"

#include <spdlog/spdlog.h>

namespace engine::ui {

UiManager::UiManager()
{
    // 创建一个无特定大小和位置的Panel，它的子元素将基于它定位。
    m_rootElement = std::make_unique<UiPanel>(glm::vec2{ 0.0f, 0.0f }, glm::vec2{ 0.0f, 0.0f });
    spdlog::trace("UI 管理器构造完成。");
}

UiManager::~UiManager() = default;

bool UiManager::init(const glm::vec2& windowSize)
{
    m_rootElement->setSize(windowSize);
    spdlog::trace("UI 管理器已初始化根面板。");
    return true;
}

} // namespace engine::ui
