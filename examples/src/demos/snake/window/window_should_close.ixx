export module demo.window.window_should_close;

import extensions.scheduler.accessors.events.Reader;

import demo.window.events;

using namespace extensions::scheduler::accessors;

namespace window {

export inline constexpr auto window_should_close =   //
    [](const ::events::Reader<window::events::CloseRequested>& closed_event_reader) {
        return closed_event_reader.read().size() > 0;
    };

}   // namespace window
