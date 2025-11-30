export module snake.app.tasks.process_events;

import ddge.modules.scheduler.accessors.events;
import ddge.modules.scheduler.primitives.as_task;
import ddge.modules.scheduler.TaskBuilder;

using namespace ddge::scheduler::accessors;

namespace app::tasks {

export [[nodiscard]]
auto process_events() -> ddge::scheduler::TaskBuilder<void>
{
    return ddge::scheduler::as_task(+[](const Processor& event_processor) -> void {
        event_processor.process_events();
    });
}

}   // namespace app::tasks
