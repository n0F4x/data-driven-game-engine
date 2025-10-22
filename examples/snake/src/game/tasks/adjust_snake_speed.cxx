module;

#include <chrono>

module snake.game.tasks.adjust_snake_speed;

import ddge.modules.ecs;

import ddge.modules.exec.accessors.ecs.Query;
import ddge.modules.exec.accessors.resources.Resource;
import ddge.modules.exec.accessors.states.State;
import ddge.modules.exec.providers.ECSProvider;
import ddge.modules.exec.providers.ResourceProvider;
import ddge.modules.exec.providers.StateProvider;
import ddge.modules.exec.v2.primitives.as_task;

import snake.game.GameState;
import snake.game.Settings;
import snake.game.Snake;

using namespace ddge::ecs::query_filter_tags;

using namespace ddge::exec::accessors::ecs;
using namespace ddge::exec::accessors::resources;
using namespace ddge::exec::accessors::states;

auto game::tasks::adjust_snake_speed()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>
{
    return ddge::exec::v2::as_task(
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
