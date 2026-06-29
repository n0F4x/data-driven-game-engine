module snake.app.schedule;

import ddge.modules.scheduler.ScheduleBuilder;
import ddge.modules.scheduler.primitives.start_as;

import snake.app.tasks.initialize;
import snake.app.tasks.run_game_loop;
import snake.app.tasks.shut_down;

namespace app {

auto schedule() -> ddge::scheduler::ScheduleBuilder
{
    return ddge::scheduler::start_as(tasks::initialize())
        .then(tasks::run_game_loop())
        .then(tasks::shut_down());
}

}   // namespace app
