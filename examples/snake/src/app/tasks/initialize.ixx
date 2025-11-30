export module snake.app.tasks.initialize;

import ddge.modules.scheduler.primitives.group;
import ddge.modules.scheduler.TaskBuilderGroup;

import snake.game.tasks.initialize;
import snake.profiler.tasks.initialize;
import snake.window.tasks.initialize;

namespace app::tasks {

export [[nodiscard]]
auto initialize() -> ddge::scheduler::TaskBuilderGroup<void>
{
    return ddge::scheduler::group(
        window::tasks::initialize(),   //
        game::tasks::initialize(),
        profiler::tasks::initialize()
    );
}

}   // namespace app::tasks
