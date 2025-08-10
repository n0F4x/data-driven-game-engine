export module snake.game.create_update_task_builder;

import ddge.modules.scheduler.TaskBuilder;

namespace game {

export [[nodiscard]]
auto create_update_task_builder() -> ddge::scheduler::TaskBuilder<void>;

}   // namespace game
