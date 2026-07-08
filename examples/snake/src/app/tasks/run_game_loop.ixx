export module snake.app.tasks.run_game_loop;

import ddge.scheduler.TaskBuilder;

namespace app::tasks {

export [[nodiscard]]
auto run_game_loop() -> ddge::scheduler::TaskBuilder<void>;

}   // namespace app::tasks
