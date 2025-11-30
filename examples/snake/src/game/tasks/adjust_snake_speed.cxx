module;

#include <chrono>

module snake.game.tasks.adjust_snake_speed;

import ddge.modules.ecs;

import ddge.modules.scheduler.accessors.ecs;
import ddge.modules.scheduler.accessors.resources;
import ddge.modules.scheduler.accessors.states;
import ddge.modules.scheduler.primitives.as_task;

import snake.game.GameState;
import snake.game.Settings;
import snake.game.Snake;

using namespace ddge::ecs::query_filter_tags;

using namespace ddge::scheduler::accessors::ecs;
using namespace ddge::scheduler::accessors::resources;
using namespace ddge::scheduler::accessors::states;

auto game::tasks::adjust_snake_speed() -> ddge::scheduler::TaskBuilder<void>
{
    return ddge::scheduler::as_task(
        +[](   //
             const Resource<Settings>           settings,
             const State<GameState>             game_state,
             Query<ddge::ecs::ID, With<Snake>>& snakes
         ) -> void   //
        {
            int number_of_snakes{ static_cast<int>(snakes.count()) };

            const int number_of_cells{
                settings->cells_per_row * settings->cells_per_column   //
            };

            // lerp := a + t(b − a)
            const auto snake_move_duration{
                game_state->default_snake_move_duration
                + (game_state->min_snake_move_duration
                   - game_state->default_snake_move_duration)
                      * (number_of_snakes - 1) / (number_of_cells - 1)   //
            };

            game_state->snake_move_timer.adjust_tick_duration(snake_move_duration);
        }
    );
}
