export module snake.game.create_update_task_builder;

import modules.scheduler.TaskBuilder;

namespace game {

export [[nodiscard]]
auto create_update_task_builder() -> modules::scheduler::TaskBuilder<void>;

}   // namespace game
