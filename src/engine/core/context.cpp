#include "context.h"
#include "../audio/audio_player.h"
#include "../input/input_manager.h"
#include "../physics/physics_engine.h"
#include "../render/camera.h"
#include "../render/renderer.h"
#include "../render/text_renderer.h"
#include "../resource/resource_manager.h"

#include <spdlog/spdlog.h>

namespace engine::core {

Context::Context(engine::input::InputManager& inputManager,
                 engine::render::Renderer& renderer,
                 engine::render::Camera& camera,
                 engine::resource::ResourceManager& resourceManager,
                 engine::physics::PhysicsEngine& physicsEngine,
                 engine::audio::AudioPlayer& audioPlayer,
                 engine::render::TextRenderer& textRenderer)
    : m_inputManager{ inputManager }
    , m_renderer{ renderer }
    , m_camera{ camera }
    , m_resourceManager{ resourceManager }
    , m_physicsEngine{ physicsEngine }
    , m_audioPlayer{ audioPlayer }
    , m_textRenderer{ textRenderer }
{
    spdlog::trace("上下文已创建并初始化，包含输入管理器、渲染器、相机、资源引擎、物理引擎、音频播放"
                  "器和文字渲染引擎。");
}

} // namespace engine::core
