export module snake.app.tasks.update;

import ddge.modules.exec.Cardinality;
import ddge.modules.exec.primitives.group;
import ddge.modules.exec.TaskBlueprint;

import snake.game.tasks.update;
import snake.window.tasks.update;

namespace app::tasks {

export [[nodiscard]]
auto update() -> ddge::exec::TaskBlueprint<void, ddge::exec::Cardinality::eMulti>
{
    return ddge::exec::group(
        window::tasks::update(),   //
        game::tasks::update()
    );
}

}   // namespace app::tasks
