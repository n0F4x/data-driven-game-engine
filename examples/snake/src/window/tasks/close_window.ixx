export module snake.window.tasks.close_window;

import ddge.modules.scheduler.TaskBuilder;

namespace window::tasks {

export [[nodiscard]]
auto close_window() -> ddge::scheduler::TaskBuilder<void>;

}   // namespace window::tasks
