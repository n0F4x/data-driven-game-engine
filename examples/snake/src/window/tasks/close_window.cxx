module snake.window.tasks.close_window;

import ddge.modules.exec.accessors.resources;
import ddge.modules.exec.v2.primitives.force_on_main;

import snake.window.Window;

using namespace ddge::exec::accessors;

auto window::tasks::close_window()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>
{
    return ddge::exec::v2::force_on_main(
        +[](const resources::Resource<Window> window) -> void {
            return window->close();
        }   //
    );
}
