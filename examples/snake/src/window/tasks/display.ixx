export module snake.window.tasks.display;

import ddge.modules.scheduler.TaskBuilder;

namespace window::tasks {

export [[nodiscard]]
auto display() -> ddge::scheduler::TaskBuilder<void>;

}   // namespace window::tasks
