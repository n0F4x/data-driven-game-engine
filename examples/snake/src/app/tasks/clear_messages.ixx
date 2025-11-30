export module snake.app.tasks.clear_messages;

import ddge.modules.exec.accessors.messages;
import ddge.modules.exec.Cardinality;
import ddge.modules.exec.primitives.as_task;
import ddge.modules.exec.TaskBlueprint;

using namespace ddge::exec::accessors;

namespace app::tasks {

export [[nodiscard]]
auto clear_messages() -> ddge::exec::TaskBlueprint<void, ddge::exec::Cardinality::eSingle>
{
    return ddge::exec::as_task(+[](const Mailbox& mailbox) -> void {
        mailbox.clear_messages();
    });
}

}   // namespace app::tasks
