#include "tilelayer_component.h"

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

} // namespace engine::component
