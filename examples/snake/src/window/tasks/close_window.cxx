module snake.window.tasks.close_window;

import ddge.modules.exec.accessors.resources;
import ddge.modules.exec.primitives.force_on_main;

import snake.window.Window;

using namespace ddge::exec::accessors;

auto window::tasks::close_window()
    -> ddge::exec::TaskBlueprint<void, ddge::exec::Cardinality::eSingle>
{
    return ddge::exec::force_on_main(
        +[](const resources::Resource<Window> window) -> void {
            return window->close();
        }   //
    );
}
