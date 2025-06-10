export module snake.game.game_tick_rate;

import core.measurement.updates_per_second;

using namespace core::measurement::literals;

namespace game {

export inline constexpr auto game_tick_rate{ 2_ups };

}   // namespace game
