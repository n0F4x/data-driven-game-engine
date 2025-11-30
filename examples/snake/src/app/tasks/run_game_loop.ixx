export module snake.app.tasks.run_game_loop;

import ddge.modules.scheduler.primitives.all_of;
import ddge.modules.scheduler.primitives.group;
import ddge.modules.scheduler.primitives.loop_until;
import ddge.modules.scheduler.primitives.not_fn;
import ddge.modules.scheduler.primitives.start_as;
import ddge.modules.scheduler.TaskBuilder;

import snake.app.tasks.clear_messages;
import snake.app.tasks.process_events;
import snake.app.tasks.render;
import snake.app.tasks.update;
import snake.game.tasks.game_is_running;
import snake.profiler.tasks.update;
import snake.window.tasks.window_should_close;

namespace app::tasks {

export [[nodiscard]]
auto run_game_loop() -> ddge::scheduler::TaskBuilder<void>
{
    return ddge::scheduler::loop_until(
        ddge::scheduler::start_as(
            ddge::scheduler::group(
                process_events(),   //
                clear_messages()
            )
        )
            .then(update())
            .then(render())
            .then(profiler::tasks::update()),
        ddge::scheduler::all_of(
            ddge::scheduler::not_fn(window::tasks::window_should_close()),   //
            game::tasks::game_is_running()
        )
    );
}

}   // namespace app::tasks
