export module snake.game.create_initialize_task_builder;

import core.scheduler.TaskBuilder;

namespace game {

export [[nodiscard]]
auto create_initialize_task_builder() -> core::scheduler::TaskBuilder<void>;

}   // namespace game
