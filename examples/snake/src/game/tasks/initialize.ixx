export module snake.game.create_initialize_task_builder;

import ddge.modules.execution.v2.TaskBuilder;

namespace game {

export [[nodiscard]]
auto initialize() -> ddge::exec::v2::TaskBuilder<void>;

}   // namespace game
