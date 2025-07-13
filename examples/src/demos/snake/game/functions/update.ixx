export module snake.game.update;

import core.scheduler;
import core.time.FixedTimer;

import extensions.scheduler.accessors.resources;

import snake.game.apple_spawn_rate;
import snake.game.color_cells;
import snake.game.eat_apple;
import snake.game.game_tick_rate;
import snake.game.move_snake;
import snake.game.spawn_apple;

using namespace extensions::scheduler::accessors;

namespace game {

inline constexpr auto update_timers =
    [](const resources::Resource<core::time::FixedTimer<game_tick_rate>> game_timer,
       const resources::Resource<core::time::FixedTimer<apple_spawn_rate>> apple_spawn_timer
    ) {   //
        game_timer->update();
        apple_spawn_timer->update();
    };

export inline constexpr auto update =
    core::scheduler::start_as(update_timers)
        .then(core::scheduler::at_fixed_rate<game_tick_rate>(move_snake))
        .then(core::scheduler::at_fixed_rate<apple_spawn_rate>(spawn_apple))
        .then(core::scheduler::at_fixed_rate<game_tick_rate>(eat_apple))
        .then(core::scheduler::at_fixed_rate<game_tick_rate>(color_cells));

}   // namespace game
