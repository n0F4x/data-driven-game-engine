export module snake.game.tasks.move_snake;

import ddge.modules.execution.v2.TaskBuilder;

namespace game {

export [[nodiscard]]
auto move_snake() -> ddge::exec::v2::TaskBuilder<void>;

}   // namespace game
