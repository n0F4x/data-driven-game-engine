export module snake.window.tasks.initialize;

import ddge.scheduler.TaskBuilder;

namespace window::tasks {

export [[nodiscard]]
auto initialize() -> ddge::scheduler::TaskBuilder<void>;

}   // namespace window::tasks
