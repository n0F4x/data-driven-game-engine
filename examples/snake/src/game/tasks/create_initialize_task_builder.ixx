export module snake.game.create_initialize_task_builder;

import ddge.modules.scheduler.TaskBuilder;

namespace game {

export [[nodiscard]]
auto create_initialize_task_builder() -> ddge::scheduler::TaskBuilder<void>;

}   // namespace game
