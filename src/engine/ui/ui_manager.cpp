#include "ui_manager.h"
#include "ui_panel.h"

#include <spdlog/spdlog.h>

namespace engine::ui {

UiManager::UiManager() {}

UiManager::~UiManager() = default;

bool UiManager::init(const glm::vec2& windowSize)
{
    m_rootElement->setSize(windowSize);
    spdlog::trace("UI 管理器已初始化根面板。");
    return true;
}

} // namespace engine::ui
