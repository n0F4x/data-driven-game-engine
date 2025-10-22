export module snake.app.tasks.update;

import ddge.modules.exec.v2.Cardinality;
import ddge.modules.exec.v2.primitives.group;
import ddge.modules.exec.v2.TaskBlueprint;

import snake.game.tasks.update;
import snake.window.tasks.update;

namespace app::tasks {

export [[nodiscard]]
auto update() -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eMulti>
{
    return ddge::exec::v2::group(
        window::tasks::update(),   //
        game::tasks::update()
    );
}

}   // namespace app::tasks
