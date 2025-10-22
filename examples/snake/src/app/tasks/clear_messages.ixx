module;

// TODO: remove once Clang can mangle
#include <function2/function2.hpp>

export module snake.app.tasks.clear_messages;

import ddge.modules.execution.accessors.messages.Mailbox;
import ddge.modules.execution.providers.MessageProvider;
import ddge.modules.execution.v2.Cardinality;
import ddge.modules.execution.v2.primitives.as_task;
import ddge.modules.execution.v2.TaskBlueprint;

using namespace ddge::exec::accessors;

namespace app {

export [[nodiscard]]
auto clear_messages()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>
{
    return ddge::exec::v2::as_task(+[](const Mailbox& mailbox) -> void {
        mailbox.clear_messages();
    });
}

}   // namespace app
