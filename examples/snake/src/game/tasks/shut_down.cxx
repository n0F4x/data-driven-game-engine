module snake.game.tasks.shut_down;

import ddge.modules.exec.accessors.states;
import ddge.modules.exec.primitives.as_task;

import snake.game.GameState;

using namespace ddge::exec::accessors;

auto game::tasks::shut_down()
    -> ddge::exec::TaskBlueprint<void, ddge::exec::Cardinality::eSingle>
{
    return ddge::exec::as_task(+[](const states::State<GameState> game_state) -> void {   //
        game_state.reset();
    });
}
