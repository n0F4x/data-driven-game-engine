module;

#include <mp-units/framework.h>

export module snake.window.display_rate;

import core.measurement.updates_per_second;

using namespace core::measurement::literals;
using namespace core::measurement::symbols;

namespace window {

export inline constexpr mp_units::quantity<ups> display_rate{ 60_ups };

}   // namespace window
