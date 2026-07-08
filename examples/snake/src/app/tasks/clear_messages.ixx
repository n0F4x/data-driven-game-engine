export module snake.app.tasks.clear_messages;

import ddge.scheduler.accessors.messages;
import ddge.scheduler.primitives.as_task;
import ddge.scheduler.TaskBuilder;

using namespace ddge::scheduler::accessors;

namespace app::tasks {

export [[nodiscard]]
auto clear_messages() -> ddge::scheduler::TaskBuilder<void>
{
    return ddge::scheduler::as_task(+[](const Mailbox& mailbox) -> void {
        mailbox.clear_messages();
    });
}

}   // namespace app::tasks
