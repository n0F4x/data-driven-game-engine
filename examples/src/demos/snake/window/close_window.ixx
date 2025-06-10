export module snake.window.close_window;

import extensions.scheduler.accessors.resources;

import snake.window.Window;

using namespace extensions::scheduler::accessors;

namespace window {

export inline constexpr auto close_window =     //
    [](const resources::Ref<Window> window) {   //
        return window->close();
    };

}   // namespace window
