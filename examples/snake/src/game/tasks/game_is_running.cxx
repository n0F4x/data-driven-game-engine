module snake.game.tasks.game_is_running;

import ddge.modules.scheduler.accessors.events;
import ddge.modules.scheduler.primitives.as_task;

import snake.game.GameOver;

using namespace ddge::scheduler::accessors;

auto game::tasks::game_is_running()
    -> ddge::scheduler::TaskBlueprint<bool, ddge::scheduler::Cardinality::eSingle>
{
    return ddge::scheduler::as_task(
        +[](const events::Reader<GameOver>& game_over_reader) -> bool {
            return game_over_reader.read().size() == 0;
        }
    );
}
