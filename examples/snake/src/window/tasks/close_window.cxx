module snake.window.tasks.close_window;

import ddge.modules.scheduler.accessors.resources;
import ddge.modules.scheduler.primitives.force_on_main;

import snake.window.Window;

using namespace ddge::scheduler::accessors;

auto window::tasks::close_window() -> ddge::scheduler::TaskBuilder<void>
{
    return ddge::scheduler::force_on_main(
        +[](const resources::Resource<Window> window) -> void {
            return window->close();
        }   //
    );
}
