module;

#include <chrono>

export module snake.game.adjust_snake_speed;

import core.ecs;
import core.measurement.updates_per_second;

import extensions.scheduler.accessors.ecs.Query;
import extensions.scheduler.accessors.resources.Resource;
import extensions.scheduler.accessors.states.State;

import snake.game.GameState;
import snake.game.Settings;
import snake.game.Snake;

using namespace core::ecs::query_parameter_tags;

using namespace extensions::scheduler::accessors::ecs;
using namespace extensions::scheduler::accessors::resources;
using namespace extensions::scheduler::accessors::states;

namespace game {

export inline constexpr auto adjust_snake_speed =
    [](const Resource<Settings>                settings,
       const State<GameState>                  game_state,
       const Query<core::ecs::ID, With<Snake>> snakes) {
        int number_of_snakes{};
        snakes.for_each([&number_of_snakes](core::ecs::ID) { number_of_snakes++; });

        const int number_of_cells{ settings->cells_per_row * settings->cells_per_column };

        // lerp := a + t(b − a)
        const auto snake_move_duration{ game_state->default_snake_move_duration
                                        + (game_state->max_snake_move_duration
                                           - game_state->default_snake_move_duration)
                                              * (number_of_snakes - 1)
                                              / (number_of_cells - 1) };

        game_state->snake_move_timer.adjust_tick_duration(snake_move_duration);
    };

}   // namespace game
