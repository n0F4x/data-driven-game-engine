export module snake.game.tasks.spawn_apple;

import ddge.modules.execution.v2.TaskBuilder;

namespace game {

export [[nodiscard]]
auto spawn_apple() -> ddge::exec::v2::TaskBuilder<void>;

}   // namespace game
