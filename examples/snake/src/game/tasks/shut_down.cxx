module snake.game.tasks.shut_down;

import ddge.modules.scheduler.accessors.states;
import ddge.modules.scheduler.primitives.as_task;

import snake.game.GameState;

using namespace ddge::scheduler::accessors;

auto game::tasks::shut_down() -> ddge::scheduler::TaskBuilder<void>
{
    return ddge::scheduler::as_task(
        +[](const states::State<GameState> game_state) -> void {   //
            game_state.reset();
        }
    );
}
