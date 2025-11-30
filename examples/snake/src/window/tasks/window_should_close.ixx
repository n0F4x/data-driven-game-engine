export module snake.window.tasks.window_should_close;

import ddge.modules.scheduler.TaskBuilder;

namespace window::tasks {

export auto window_should_close() -> ddge::scheduler::TaskBuilder<bool>;

}   // namespace window::tasks
