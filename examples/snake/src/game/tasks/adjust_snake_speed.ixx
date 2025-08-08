module;

#include <chrono>

export module snake.game.adjust_snake_speed;

import modules.ecs;

import modules.scheduler.accessors.ecs.Query;
import modules.scheduler.accessors.resources.Resource;
import modules.scheduler.accessors.states.State;

import snake.game.GameState;
import snake.game.Settings;
import snake.game.Snake;

using namespace modules::ecs::query_parameter_tags;

using namespace modules::scheduler::accessors::ecs;
using namespace modules::scheduler::accessors::resources;
using namespace modules::scheduler::accessors::states;

namespace game {

export auto adjust_snake_speed(
    Resource<Settings>                 settings,
    State<GameState>                   game_state,
    Query<modules::ecs::ID, With<Snake>>& snakes
) -> void;

}   // namespace game

module :private;

auto game::adjust_snake_speed(
    const Resource<Settings>           settings,
    const State<GameState>             game_state,
    Query<modules::ecs::ID, With<Snake>>& snakes
) -> void
{
    int number_of_snakes{};
    snakes.for_each([&number_of_snakes](modules::ecs::ID) { number_of_snakes++; });

    const int number_of_cells{ settings->cells_per_row * settings->cells_per_column };

    // lerp := a + t(b − a)
    const auto snake_move_duration{
        game_state->default_snake_move_duration
        + (game_state->max_snake_move_duration - game_state->default_snake_move_duration)
              * (number_of_snakes - 1) / (number_of_cells - 1)
    };

    game_state->snake_move_timer.adjust_tick_duration(snake_move_duration);
}
