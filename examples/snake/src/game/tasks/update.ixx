export module snake.game.create_update_task_builder;

import ddge.modules.execution.v2.TaskBuilder;

namespace game {

export [[nodiscard]]
auto update() -> ddge::exec::v2::TaskBuilder<void>;

}   // namespace game
