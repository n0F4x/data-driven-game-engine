export module snake.app.tasks.initialize;

import ddge.modules.exec.Cardinality;
import ddge.modules.exec.primitives.group;
import ddge.modules.exec.TaskBlueprint;

import snake.game.tasks.initialize;
import snake.profiler.tasks.initialize;
import snake.window.tasks.initialize;

namespace app::tasks {

export [[nodiscard]]
auto initialize() -> ddge::exec::TaskBlueprint<void, ddge::exec::Cardinality::eMulti>
{
    return ddge::exec::group(
        window::tasks::initialize(),   //
        game::tasks::initialize(),
        profiler::tasks::initialize()
    );
}

}   // namespace app::tasks
