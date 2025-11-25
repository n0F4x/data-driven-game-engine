export module snake.app.tasks.clear_messages;

import ddge.modules.exec.accessors.messages;
import ddge.modules.exec.v2.Cardinality;
import ddge.modules.exec.v2.primitives.as_task;
import ddge.modules.exec.v2.TaskBlueprint;

using namespace ddge::exec::accessors;

namespace app::tasks {

export [[nodiscard]]
auto clear_messages()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>
{
    return ddge::exec::v2::as_task(+[](const Mailbox& mailbox) -> void {
        mailbox.clear_messages();
    });
}

}   // namespace app::tasks
