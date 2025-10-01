export module snake.game.tasks.eat_apple;

import ddge.modules.execution.v2.TaskBuilder;

namespace game {

export [[nodiscard]]
auto eat_apple() -> ddge::exec::v2::TaskBuilder<void>;

}   // namespace game
