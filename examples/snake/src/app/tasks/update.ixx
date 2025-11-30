export module snake.app.tasks.update;

import ddge.modules.scheduler.primitives.group;
import ddge.modules.scheduler.TaskBuilderBundle;

import snake.game.tasks.update;
import snake.window.tasks.update;

namespace app::tasks {

export [[nodiscard]]
auto update() -> ddge::scheduler::TaskBuilderBundle<void>
{
    return ddge::scheduler::group(
        window::tasks::update(),   //
        game::tasks::update()
    );
}

}   // namespace app::tasks
