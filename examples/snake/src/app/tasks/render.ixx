export module snake.app.tasks.render;

import ddge.modules.scheduler.primitives.at_fixed_rate;
import ddge.modules.scheduler.primitives.start_as;
import ddge.modules.scheduler.TaskBuilder;

import snake.game.tasks.draw;
import snake.window.DisplayTimer;
import snake.window.tasks.clear_window;
import snake.window.tasks.display;

namespace app::tasks {

export [[nodiscard]]
auto render() -> ddge::scheduler::TaskBuilder<void>
{
    return ddge::scheduler::at_fixed_rate<window::DisplayTimer>(
        ddge::scheduler::start_as(window::tasks::clear_window())
            .then(game::tasks::draw())
            .then(window::tasks::display())
    );
}

}   // namespace app::tasks
