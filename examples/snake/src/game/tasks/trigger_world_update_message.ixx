export module snake.game.tasks.trigger_world_update_message;

import ddge.modules.execution.v2.Cardinality;
import ddge.modules.execution.v2.TaskBlueprint;

namespace game {

export [[nodiscard]]
auto trigger_world_update_message()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>;

}   // namespace game
