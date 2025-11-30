export module snake.app.tasks.render;

import ddge.modules.exec.Cardinality;
import ddge.modules.exec.primitives.at_fixed_rate;
import ddge.modules.exec.primitives.start_as;
import ddge.modules.exec.TaskBlueprint;

import snake.game.tasks.draw;
import snake.window.DisplayTimer;
import snake.window.tasks.clear_window;
import snake.window.tasks.display;

namespace app::tasks {

export [[nodiscard]]
auto render() -> ddge::exec::TaskBlueprint<void, ddge::exec::Cardinality::eSingle>
{
    return ddge::exec::at_fixed_rate<window::DisplayTimer>(
        ddge::exec::start_as(window::tasks::clear_window())
            .then(game::tasks::draw())
            .then(window::tasks::display())
    );
}

}   // namespace app::tasks
