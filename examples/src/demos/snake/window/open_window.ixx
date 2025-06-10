export module demo.window.open_window;

import extensions.scheduler.accessors.resources;

import demo.window.Window;

using namespace extensions::scheduler::accessors;

namespace window {

export inline constexpr auto open_window =      //
    [](const resources::Ref<Window> window) {   //
        return window->open();
    };

}   // namespace window
