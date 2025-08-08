module;

#include <functional>
#include <span>

export module core.scheduler.accessors.events.Reader;

import core.events.event_c;
import core.events.BufferedEventQueue;

namespace core::scheduler::accessors {

inline namespace events {

export template <core::events::event_c Event_T>
class Reader {
public:
    using Event = Event_T;

    constexpr explicit Reader(
        const core::events::BufferedEventQueue<Event_T>& buffered_event_queue
    );

    [[nodiscard]]
    constexpr auto read() const -> std::span<const Event_T>;

private:
    std::reference_wrapper<const core::events::BufferedEventQueue<Event_T>>
        m_buffered_event_queue_ref;
};

}   // namespace events

}   // namespace core::scheduler::accessors

template <core::events::event_c Event_T>
constexpr core::scheduler::accessors::events::Reader<Event_T>::Reader(
    const core::events::BufferedEventQueue<Event_T>& buffered_event_queue
)
    : m_buffered_event_queue_ref{ buffered_event_queue }
{}

template <core::events::event_c Event_T>
constexpr auto core::scheduler::accessors::events::Reader<Event_T>::read() const
    -> std::span<const Event_T>
{
    return core::events::view(m_buffered_event_queue_ref.get());
}
