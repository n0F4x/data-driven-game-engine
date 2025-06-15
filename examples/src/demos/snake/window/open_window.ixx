export module snake.window.open_window;

import extensions.scheduler.accessors.resources;

import snake.window.Window;

using namespace extensions::scheduler::accessors;

namespace window {

export inline constexpr auto open_window =      //
    [](const resources::Resource<Window> window) {   //
        return window->isOpen();
    };

}   // namespace window
