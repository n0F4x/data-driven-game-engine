export module snake.app.tasks.process_events;

import ddge.modules.exec.accessors.events;
import ddge.modules.exec.Cardinality;
import ddge.modules.exec.primitives.as_task;
import ddge.modules.exec.TaskBlueprint;

using namespace ddge::exec::accessors;

namespace app::tasks {

export [[nodiscard]]
auto process_events() -> ddge::exec::TaskBlueprint<void, ddge::exec::Cardinality::eSingle>
{
    return ddge::exec::as_task(+[](const Processor& event_processor) -> void {
        event_processor.process_events();
    });
}

}   // namespace app::tasks
