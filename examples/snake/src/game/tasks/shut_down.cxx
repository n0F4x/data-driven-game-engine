module snake.game.tasks.shut_down;

import ddge.modules.execution.accessors.states.State;
import ddge.modules.execution.providers.StateProvider;
import ddge.modules.execution.v2.primitives.as_task;

import snake.game.GameState;

using namespace ddge::exec::accessors;

auto game::tasks::shut_down()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>
{
    return ddge::exec::v2::as_task(
        +[](const states::State<GameState> game_state) -> void {   //
            game_state.reset();
        }
    );
}
