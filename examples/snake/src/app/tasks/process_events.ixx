module;

// TODO: remove once Clang can mangle
#include <function2/function2.hpp>

export module snake.app.tasks.process_events;

import ddge.modules.exec.accessors.events.Processor;
import ddge.modules.exec.providers.EventProvider;
import ddge.modules.exec.v2.Cardinality;
import ddge.modules.exec.v2.primitives.as_task;
import ddge.modules.exec.v2.TaskBlueprint;

using namespace ddge::exec::accessors;

namespace app::tasks {

export [[nodiscard]]
auto process_events()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>
{
    return ddge::exec::v2::as_task(+[](const Processor& event_processor) -> void {
        event_processor.process_events();
    });
}

}   // namespace app::tasks
