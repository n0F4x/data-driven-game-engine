export module snake.app.tasks.render;

import ddge.modules.exec.v2.Cardinality;
import ddge.modules.exec.v2.primitives.at_fixed_rate;
import ddge.modules.exec.v2.primitives.start_as;
import ddge.modules.exec.v2.TaskBlueprint;

import snake.game.tasks.draw;
import snake.window.DisplayTimer;
import snake.window.tasks.clear_window;
import snake.window.tasks.display;

namespace app::tasks {

export [[nodiscard]]
auto render() -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>
{
    return ddge::exec::v2::at_fixed_rate<window::DisplayTimer>(
        ddge::exec::v2::start_as(window::tasks::clear_window())
            .then(game::tasks::draw())
            .then(window::tasks::display())
    );
}

}   // namespace app::tasks
