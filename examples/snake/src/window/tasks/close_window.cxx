module snake.window.tasks.close_window;

import ddge.modules.execution.accessors.resources;
import ddge.modules.execution.providers.ResourceProvider;
import ddge.modules.execution.v2.primitives.as_task;

import snake.window.Window;

using namespace ddge::exec::accessors;

auto window::tasks::close_window()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>
{
    return ddge::exec::v2::as_task(
        +[](const resources::Resource<Window> window) -> void {
            return window->close();
        }   //
    );
}
