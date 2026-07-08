export module snake.app.tasks.shut_down;

import ddge.scheduler.TaskBuilder;

namespace app::tasks {

export [[nodiscard]]
auto shut_down() -> ddge::scheduler::TaskBuilder<void>;

}   // namespace app::tasks
