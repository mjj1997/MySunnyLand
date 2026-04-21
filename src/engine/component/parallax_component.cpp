#include "parallax_component.h"
#include "../component/transform_component.h"
#include "../core/context.h"
#include "../object/game_object.h"
#include "../render/renderer.h"

#include <spdlog/spdlog.h>

namespace engine::component {

ParallaxComponent::ParallaxComponent(const std::string& textureId,
                                     const glm::vec2& scrollFactor,
                                     const glm::bvec2& repeat)
    : m_sprite{ engine::render::Sprite{ textureId } }
    , m_scrollFactor{ scrollFactor }
    , m_repeat{ repeat }
{
    spdlog::trace("ParallaxComponent 初始化完成， 纹理 ID： {}", textureId);
}

void ParallaxComponent::init()
{
    if (!m_owner) {
        spdlog::error("ParallaxComponent 初始化时，GameObject 为空。");
        return;
    }

    m_transformComponent = m_owner->getComponent<TransformComponent>();
    if (!m_transformComponent) {
        spdlog::error(
            "ParallaxComponent 初始化时，GameObject 上没有找到 TransformComponent 组件。");
        return;
    }
}

void ParallaxComponent::render(engine::core::Context& context)
{
    if (m_isHidden || !m_transformComponent) {
        return;
    }

    // 直接调用视差滚动绘制函数
    context.renderer().drawParallax(context.camera(),
                                    m_sprite,
                                    m_transformComponent->position(),
                                    m_scrollFactor,
                                    m_repeat,
                                    m_transformComponent->scale());
}

} // namespace engine::component
