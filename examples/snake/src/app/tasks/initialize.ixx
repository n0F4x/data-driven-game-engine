export module snake.app.tasks.initialize;

import ddge.modules.execution.v2.Cardinality;
import ddge.modules.execution.v2.primitives.group;
import ddge.modules.execution.v2.TaskBlueprint;

import snake.game.tasks.initialize;
import snake.profiler.tasks.initialize;
import snake.window.tasks.initialize;

namespace app::tasks {

export [[nodiscard]]
auto initialize()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eMulti>
{
    return ddge::exec::v2::group(
        window::tasks::initialize(),   //
        game::tasks::initialize(),
        profiler::tasks::initialize()
    );
}

}   // namespace app::tasks
