module;

#include <concepts>
#include <functional>

export module extensions.scheduler.accessors.events.Recorder;

import core.events.event_c;
import core.events.EventQueue;

namespace extensions::scheduler::accessors::events {

export template <core::events::event_c Event_T>
class Recorder {
public:
    constexpr explicit Recorder(core::events::EventQueue<Event_T>& event_queue);

    template <typename... Args_T>
        requires std::constructible_from<Event_T, Args_T&&...>
    constexpr auto record(Args_T&&... args) const -> void;

private:
    std::reference_wrapper<core::events::EventQueue<Event_T>> m_event_queue_ref;
};

}   // namespace extensions::scheduler::accessors::events

template <core::events::event_c Event_T>
constexpr extensions::scheduler::accessors::events::Recorder<Event_T>::Recorder(
    core::events::EventQueue<Event_T>& event_queue
)
    : m_event_queue_ref{ event_queue }
{}

template <core::events::event_c Event_T>
template <typename... Args_T>
    requires std::constructible_from<Event_T, Args_T&&...>
constexpr auto extensions::scheduler::accessors::events::Recorder<Event_T>::record(
    Args_T&&... args
) const -> void
{
    m_event_queue_ref.get().emplace_back(std::forward<Args_T>(args)...);
}
