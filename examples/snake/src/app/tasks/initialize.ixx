export module snake.app.tasks.initialize;

import ddge.modules.scheduler.Cardinality;
import ddge.modules.scheduler.primitives.group;
import ddge.modules.scheduler.TaskBlueprint;

import snake.game.tasks.initialize;
import snake.profiler.tasks.initialize;
import snake.window.tasks.initialize;

namespace app::tasks {

export [[nodiscard]]
auto initialize()
    -> ddge::scheduler::TaskBlueprint<void, ddge::scheduler::Cardinality::eMulti>
{
    return ddge::scheduler::group(
        window::tasks::initialize(),   //
        game::tasks::initialize(),
        profiler::tasks::initialize()
    );
}

}   // namespace app::tasks
