export module snake.app.tasks.run_game_loop;

import ddge.modules.exec.Cardinality;
import ddge.modules.exec.primitives.all_of;
import ddge.modules.exec.primitives.group;
import ddge.modules.exec.primitives.loop_until;
import ddge.modules.exec.primitives.not_fn;
import ddge.modules.exec.primitives.start_as;
import ddge.modules.exec.TaskBlueprint;

import snake.app.tasks.clear_messages;
import snake.app.tasks.process_events;
import snake.app.tasks.render;
import snake.app.tasks.update;
import snake.game.tasks.game_is_running;
import snake.profiler.tasks.update;
import snake.window.tasks.window_should_close;

namespace app::tasks {

export [[nodiscard]]
auto run_game_loop() -> ddge::exec::TaskBlueprint<void, ddge::exec::Cardinality::eSingle>
{
    return ddge::exec::loop_until(
        ddge::exec::start_as(
            ddge::exec::group(
                process_events(),   //
                clear_messages()
            )
        )
            .then(update())
            .then(render())
            .then(profiler::tasks::update()),
        ddge::exec::all_of(
            ddge::exec::not_fn(window::tasks::window_should_close()),   //
            game::tasks::game_is_running()
        )
    );
}

}   // namespace app::tasks
