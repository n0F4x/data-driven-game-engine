module snake.game.tasks.game_is_running;

import ddge.modules.exec.accessors.events;
import ddge.modules.exec.primitives.as_task;

import snake.game.GameOver;

using namespace ddge::exec::accessors;

auto game::tasks::game_is_running()
    -> ddge::exec::TaskBlueprint<bool, ddge::exec::Cardinality::eSingle>
{
    return ddge::exec::as_task(
        +[](const events::Reader<GameOver>& game_over_reader) -> bool {
            return game_over_reader.read().size() == 0;
        }
    );
}
