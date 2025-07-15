export module snake.window.display_rate;

import mp_units.core;

import core.measurement.frames_per_second;

using namespace core::measurement::symbols;

namespace window {

export inline constexpr mp_units::quantity<fps> display_rate{ 60 * fps };

}   // namespace window
