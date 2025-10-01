module snake.game.tasks.game_is_running;

import ddge.modules.execution.accessors.events.Reader;
import ddge.modules.execution.providers.EventProvider;
import ddge.modules.execution.v2.primitives.as_task;

import snake.game.GameOver;

using namespace ddge::exec::accessors;

auto game::game_is_running() -> ddge::exec::v2::TaskBuilder<bool>
{
    return ddge::exec::v2::as_task(
        +[](const events::Reader<GameOver>& game_over_reader) -> bool {
            return game_over_reader.read().size() == 0;
        }
    );
}
