export module snake.game.tasks.trigger_world_update_message;

import ddge.modules.execution.v2.TaskBuilder;

namespace game {

export [[nodiscard]]
auto trigger_world_update_message() -> ddge::exec::v2::TaskBuilder<void>;

}   // namespace game
