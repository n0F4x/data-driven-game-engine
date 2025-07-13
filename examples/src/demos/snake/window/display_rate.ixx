export module snake.window.display_rate;

import mp_units.core;

import core.measurement.updates_per_second;

using namespace core::measurement::symbols;

namespace window {

export inline constexpr mp_units::quantity<ups> display_rate{ 60 * ups };

}   // namespace window
