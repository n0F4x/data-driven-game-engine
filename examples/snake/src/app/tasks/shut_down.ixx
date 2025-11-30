export module snake.app.tasks.shut_down;

import ddge.modules.scheduler.Cardinality;
import ddge.modules.scheduler.primitives.start_as;
import ddge.modules.scheduler.TaskBlueprint;

import snake.game.tasks.shut_down;
import snake.window.tasks.close_window;

namespace app::tasks {

export [[nodiscard]]
auto shut_down()
    -> ddge::scheduler::TaskBlueprint<void, ddge::scheduler::Cardinality::eSingle>
{
    return ddge::scheduler::start_as(game::tasks::shut_down())   //
        .then(window::tasks::close_window());
}

}   // namespace app::tasks
