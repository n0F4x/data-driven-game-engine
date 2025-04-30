module;

#include <iostream>

export module demo.window.Window;

import demo.window.events;
import demo.window.Settings;

import extensions.scheduler.accessors.events.Recorder;

namespace window {

export using EventRecorder =
    extensions::scheduler::accessors::events::Recorder<events::CloseRequested>;

export class Window {
public:
    constexpr explicit Window(const Settings&) {}

    auto open() -> void
    {
        std::puts("Window is open");
        m_is_open = true;
    }

    [[nodiscard]]
    auto is_open() const -> bool
    {
        return m_is_open;
    }

    auto close() -> void
    {
        std::puts("Window is closed");
        m_is_open = false;
    }

    auto record_events(EventRecorder event_recorder);

private:
    bool m_is_open{};
};

}   // namespace window

auto window::Window::record_events(const ::window::EventRecorder event_recorder)
{
    std::puts("Closed event recorded");
    event_recorder.record();
}
