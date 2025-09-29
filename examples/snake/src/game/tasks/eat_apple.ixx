export module snake.game.create_eat_apple_task_builder;

import ddge.modules.execution.v2.TaskBuilder;

namespace game {

export [[nodiscard]]
auto eat_apple() -> ddge::exec::v2::TaskBuilder<void>;

}   // namespace game
