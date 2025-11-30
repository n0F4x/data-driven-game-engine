export module snake.profiler.tasks.initialize;

import ddge.modules.scheduler.TaskBuilder;

namespace profiler::tasks {

export [[nodiscard]]
auto initialize() -> ddge::scheduler::TaskBuilder<void>;

}   // namespace profiler::tasks
