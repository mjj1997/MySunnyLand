#include "tilelayer_component.h"
#include "../core/context.h"
#include "../render/renderer.h"

#include <spdlog/spdlog.h>

namespace engine::component {

TileLayerComponent::TileLayerComponent(glm::ivec2 tileSize,
                                       glm::ivec2 mapSize,
                                       std::vector<TileInfo>&& tiles)
    : m_tileSize{ tileSize }
    , m_mapSize{ mapSize }
    , m_tiles{ std::move(tiles) }
{
    if (m_tiles.size() != static_cast<size_t>(m_mapSize.x * m_mapSize.y)) {
        spdlog::error("TileLayerComponent: 地图尺寸与提供的瓦片向量大小不匹配。瓦片数据将被清除。");
        m_tiles.clear();
        m_mapSize = glm::ivec2{ 0, 0 };
    }
    spdlog::trace("TileLayerComponent 构造完成");
}

const TileInfo* TileLayerComponent::tileInfoAt(glm::ivec2 pos) const
{
    if (pos.x < 0 || pos.x >= m_mapSize.x || pos.y < 0 || pos.y >= m_mapSize.y) {
        spdlog::warn("TileLayerComponent: 瓦片坐标越界: ({}, {})", pos.x, pos.y);
        return nullptr;
    }

    const auto index = static_cast<size_t>(pos.y * m_mapSize.x + pos.x);
    if (index >= m_tiles.size()) {
        spdlog::warn("TileLayerComponent: 索引超出瓦片向量范围: {}", index);
        return nullptr;
    }

    return &m_tiles.at(index);
}

void TileLayerComponent::init()
{
    if (!m_owner) {
        spdlog::warn("TileLayerComponent 的 m_owner 未设置。");
    }
    spdlog::trace("TileLayerComponent 初始化完成");
}

void TileLayerComponent::render(engine::core::Context& context)
{
    if (m_tileSize.x <= 0 || m_tileSize.y <= 0) {
        return; // 防止除以零或无效尺寸
    }

    // 遍历所有瓦片
    for (int y{ 0 }; y < m_mapSize.y; ++y) {
        for (int x{ 0 }; x < m_mapSize.x; ++x) {
            size_t index{ static_cast<size_t>(y * m_mapSize.x + x) };
            // 检查索引有效性以及瓦片是否需要渲染
            if (index < m_tiles.size() && m_tiles[index].type != TileType::Empty) {
                const auto& tileInfo = m_tiles[index];
                // 计算该瓦片在世界中的左上角位置 (drawSprite 预期接收左上角坐标)
                glm::vec2 tileTopLeftPos{ m_offset.x + static_cast<float>(x) * m_tileSize.x,
                                          m_offset.y + static_cast<float>(y) * m_tileSize.y };

                // 但如果图片的大小与瓦片的大小不一致，需要调整 y 坐标 (瓦片与图片的对齐点是左下角)
                const auto spriteHeight = tileInfo.sprite.sourceRect()->h;
                if (static_cast<int>(spriteHeight) != m_tileSize.y) {
                    // 计算 y 坐标偏移量(从图片左上角到瓦片左上角之间的距离，即图片高度 - 瓦片高度)
                    const auto offsetY = spriteHeight - static_cast<float>(m_tileSize.y);
                    tileTopLeftPos.y -= offsetY;
                }

                // 执行绘制
                context.renderer().drawSprite(context.camera(), tileInfo.sprite, tileTopLeftPos);
            }
        }
    }
}

} // namespace engine::component
