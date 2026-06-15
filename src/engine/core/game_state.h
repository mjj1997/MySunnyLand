#pragma once

namespace engine::core {

/**
 * @enum State
 * @brief 定义游戏可能处于的宏观状态。
 */
enum class State {
    InTitle,  ///< @brief 标题界面
    Playing,  ///< @brief 正常游戏进行中
    Paused,   ///< @brief 游戏暂停（通常覆盖菜单界面）
    GameOver, ///< @brief 游戏结束界面
    // 可以根据需要添加更多状态，如 Cutscene, SettingsMenu 等
};

} // namespace engine::core
