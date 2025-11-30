export module snake.game.tasks.trigger_world_update_message;

import ddge.modules.scheduler.Cardinality;
import ddge.modules.scheduler.TaskBlueprint;

namespace game::tasks {

export [[nodiscard]]
auto trigger_world_update_message()
    -> ddge::scheduler::TaskBlueprint<void, ddge::scheduler::Cardinality::eSingle>;

}   // namespace game::tasks
