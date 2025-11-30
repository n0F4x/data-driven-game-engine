export module snake.app.tasks.clear_messages;

import ddge.modules.scheduler.accessors.messages;
import ddge.modules.scheduler.Cardinality;
import ddge.modules.scheduler.primitives.as_task;
import ddge.modules.scheduler.TaskBlueprint;

using namespace ddge::scheduler::accessors;

namespace app::tasks {

export [[nodiscard]]
auto clear_messages()
    -> ddge::scheduler::TaskBlueprint<void, ddge::scheduler::Cardinality::eSingle>
{
    return ddge::scheduler::as_task(+[](const Mailbox& mailbox) -> void {
        mailbox.clear_messages();
    });
}

}   // namespace app::tasks
