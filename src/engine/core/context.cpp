#include "context.h"
#include <../input/input_manager.h>
#include <../render/camera.h>
#include <../render/renderer.h>
#include <../resource/resource_manager.h>

#include <spdlog/spdlog.h>

namespace engine::core {

Context::Context(engine::input::InputManager& inputManager,
                 engine::render::Renderer& renderer,
                 engine::render::Camera& camera,
                 engine::resource::ResourceManager& resourceManager)
    : m_inputManager{ inputManager }
    , m_renderer{ renderer }
    , m_camera{ camera }
    , m_resourceManager{ resourceManager }
{
    spdlog::trace("上下文已创建并初始化，包含输入管理器、渲染器、相机和资源管理器。");
}

} // namespace engine::core
