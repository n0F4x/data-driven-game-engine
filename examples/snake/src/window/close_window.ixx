export module snake.window.close_window;

import ddge.modules.scheduler.accessors.resources;

import snake.window.Window;

using namespace ddge::scheduler::accessors;

namespace window {

export auto close_window(const resources::Resource<Window> window) -> void;

}   // namespace window

module :private;

auto window::close_window(const resources::Resource<Window> window) -> void
{
    return window->close();
}
