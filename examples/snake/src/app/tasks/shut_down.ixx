export module snake.app.tasks.shut_down;

import ddge.modules.scheduler.primitives.start_as;
import ddge.modules.scheduler.TaskBuilder;

import snake.game.tasks.shut_down;
import snake.window.tasks.close_window;

namespace app::tasks {

export [[nodiscard]]
auto shut_down() -> ddge::scheduler::TaskBuilder<void>
{
    return ddge::scheduler::start_as(game::tasks::shut_down())   //
        .then(window::tasks::close_window());
}

}   // namespace app::tasks
