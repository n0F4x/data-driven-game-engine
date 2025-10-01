export module snake.game.tasks.game_is_running;

import ddge.modules.execution.v2.TaskBuilder;

namespace game {

export [[nodiscard]]
auto game_is_running() -> ddge::exec::v2::TaskBuilder<bool>;

}   // namespace game
