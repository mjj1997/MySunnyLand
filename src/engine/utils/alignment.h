namespace engine::utils {

/**
 * @enum class Alignment
 * @brief 定义对象或组件相对于其位置参考点的对齐方式。
 */
enum class Alignment {
    None,         // 不指定对齐方式，偏移量通常为 (0,0) 或手动设置
    TopLeft,      // 左上角
    TopCenter,    // 顶部中心
    TopRight,     // 右上角
    CenterLeft,   // 中心左侧
    Center,       // 正中心 (几何中心)
    CenterRight,  // 中心右侧
    BottomLeft,   // 左下角
    BottomCenter, // 底部中心
    BottomRight,  // 右下角
};

} // namespace engine::utils
