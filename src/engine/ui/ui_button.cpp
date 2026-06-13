#include "ui_button.h"

namespace engine::ui {

void UiButton::clicked()
{
    if (m_callback) {
        m_callback();
    }
}

} // namespace engine::ui
