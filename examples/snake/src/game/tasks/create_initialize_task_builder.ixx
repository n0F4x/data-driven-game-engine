export module snake.game.create_initialize_task_builder;

import ddge.modules.execution.TaskBuilder;

namespace game {

export [[nodiscard]]
auto create_initialize_task_builder() -> ddge::exec::TaskBuilder<void>;

}   // namespace game
