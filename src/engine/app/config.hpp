#pragma once

#include "engine/renderer/Renderer.hpp"
#include "engine/window/Window.hpp"

namespace engine::app::config {

using Renderer = Renderer<Window>;
using Window   = Window;

}   // namespace engine::app::config
