export module snake.game.create_update_task_builder;

import core.scheduler.TaskBuilder;

namespace game {

export [[nodiscard]]
auto create_update_task_builder() -> core::scheduler::TaskBuilder<void>;

}   // namespace game
