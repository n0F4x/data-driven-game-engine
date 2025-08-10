export module snake.game.create_update_task_builder;

import ddge.modules.execution.TaskBuilder;

namespace game {

export [[nodiscard]]
auto create_update_task_builder() -> ddge::exec::TaskBuilder<void>;

}   // namespace game
