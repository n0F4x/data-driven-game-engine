module;

// TODO: remove once Clang can mangle
#include <function2/function2.hpp>

export module snake.app.tasks.run_game_loop;

import ddge.modules.execution.v2.Cardinality;
import ddge.modules.execution.v2.primitives.all_of;
import ddge.modules.execution.v2.primitives.group;
import ddge.modules.execution.v2.primitives.loop_until;
import ddge.modules.execution.v2.primitives.not_fn;
import ddge.modules.execution.v2.primitives.start_as;
import ddge.modules.execution.v2.TaskBlueprint;

import snake.app.tasks.clear_messages;
import snake.app.tasks.process_events;
import snake.app.tasks.render;
import snake.app.tasks.update;
import snake.game.tasks.game_is_running;
import snake.profiler.tasks.update;
import snake.window.tasks.window_should_close;

namespace app {

export [[nodiscard]]
auto run_game_loop()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>
{
    return ddge::exec::v2::loop_until(
        ddge::exec::v2::start_as(
            ddge::exec::v2::group(
                process_events(),   //
                clear_messages()
            )
        )
            .then(update())
            .then(render())
            .then(profiler::update()),
        ddge::exec::v2::all_of(
            ddge::exec::v2::not_fn(window::window_should_close()),   //
            game::game_is_running()
        )
    );
}

}   // namespace app
