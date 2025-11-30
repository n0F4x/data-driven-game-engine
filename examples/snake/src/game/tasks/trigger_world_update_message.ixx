export module snake.game.tasks.trigger_world_update_message;

import ddge.modules.scheduler.TaskBuilder;

namespace game::tasks {

export [[nodiscard]]
auto trigger_world_update_message() -> ddge::scheduler::TaskBuilder<void>;

}   // namespace game::tasks
