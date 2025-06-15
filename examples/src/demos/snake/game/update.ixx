export module snake.game.update;

import core.scheduler;
import core.time.FixedTimer;

import extensions.scheduler.accessors.resources;

import snake.game.color_cells;
import snake.game.game_tick_rate;
import snake.game.move_snake;

using namespace extensions::scheduler::accessors;

namespace game {

inline constexpr auto update_game_time =
    [](const resources::Resource<core::time::FixedTimer<game_tick_rate>> game_timer) {   //
        game_timer->update();
    };

export inline constexpr auto update =
    core::scheduler::start_as(update_game_time)
        .then(
            core::scheduler::at_fixed_rate<game_tick_rate>(
                core::scheduler::start_as(move_snake)   //
                    .then(color_cells)
            )
        );

}   // namespace game
