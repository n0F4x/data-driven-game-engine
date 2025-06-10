module;

#include <mp-units/framework.h>

export module snake.game.game_tick_rate;

import core.measurement.updates_per_second;

using namespace core::measurement::literals;
using namespace core::measurement::symbols;

namespace game {

export inline constexpr mp_units::quantity<ups> game_tick_rate{ 2_ups };

}   // namespace game
