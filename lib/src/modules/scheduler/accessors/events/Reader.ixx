module;

#include <functional>
#include <span>

export module modules.scheduler.accessors.events.Reader;

import modules.events.event_c;
import modules.events.BufferedEventQueue;

namespace modules::scheduler::accessors {

inline namespace events {

export template <modules::events::event_c Event_T>
class Reader {
public:
    using Event = Event_T;

    constexpr explicit Reader(
        const modules::events::BufferedEventQueue<Event_T>& buffered_event_queue
    );

    [[nodiscard]]
    constexpr auto read() const -> std::span<const Event_T>;

private:
    std::reference_wrapper<const modules::events::BufferedEventQueue<Event_T>>
        m_buffered_event_queue_ref;
};

}   // namespace events

}   // namespace modules::scheduler::accessors

template <modules::events::event_c Event_T>
constexpr modules::scheduler::accessors::events::Reader<Event_T>::Reader(
    const modules::events::BufferedEventQueue<Event_T>& buffered_event_queue
)
    : m_buffered_event_queue_ref{ buffered_event_queue }
{}

template <modules::events::event_c Event_T>
constexpr auto modules::scheduler::accessors::events::Reader<Event_T>::read() const
    -> std::span<const Event_T>
{
    return modules::events::view(m_buffered_event_queue_ref.get());
}
