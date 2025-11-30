export module snake.window.tasks.clear_window;

import ddge.modules.scheduler.TaskBuilder;

namespace window::tasks {

export [[nodiscard]]
auto clear_window() -> ddge::scheduler::TaskBuilder<void>;

}   // namespace window::tasks
