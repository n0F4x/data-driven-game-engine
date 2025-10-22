module;

// TODO: remove once Clang can mangle
#include <function2/function2.hpp>

module snake.game.tasks.game_is_running;

import ddge.modules.exec.accessors.events.Reader;
import ddge.modules.exec.providers.EventProvider;
import ddge.modules.exec.v2.primitives.as_task;

import snake.game.GameOver;

using namespace ddge::exec::accessors;

auto game::tasks::game_is_running()
    -> ddge::exec::v2::TaskBlueprint<bool, ddge::exec::v2::Cardinality::eSingle>
{
    return ddge::exec::v2::as_task(
        +[](const events::Reader<GameOver>& game_over_reader) -> bool {
            return game_over_reader.read().size() == 0;
        }
    );
}
