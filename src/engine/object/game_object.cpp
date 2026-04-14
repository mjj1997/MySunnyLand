#include "game_object.h"

#include <spdlog/spdlog.h>

namespace engine::object {

GameObject::GameObject()
{
    spdlog::trace("GameObject created.");
}

} // namespace engine::object
