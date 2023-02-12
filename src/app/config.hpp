#pragma once

#include <cstdint>

#include "engine/renderer/Renderer.hpp"
#include "schedule/Schedule.hpp"

namespace app::config {

using Renderer = engine::Renderer;

template <class AppViewType>
using Schedule = Schedule<AppViewType>;

}   // namespace app::config
