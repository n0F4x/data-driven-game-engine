export module snake.game.apple_spawn_rate;

import mp_units.core;

import core.measurement.updates_per_second;

using namespace core::measurement::symbols;

namespace game {

export inline constexpr mp_units::quantity<ups> apple_spawn_rate{ 1.0 / 3.0 * ups };

}   // namespace game
