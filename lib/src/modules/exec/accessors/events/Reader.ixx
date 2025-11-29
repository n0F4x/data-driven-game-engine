module;

#include <functional>
#include <span>

export module ddge.modules.exec.accessors.events.Reader;

import ddge.modules.events.event_c;
import ddge.modules.events.BufferedEventQueue;
import ddge.modules.exec.accessors.events.Event;
import ddge.modules.exec.accessors.events.EventManager;
import ddge.modules.exec.locks.CriticalSectionType;
import ddge.modules.exec.locks.Lock;
import ddge.modules.exec.locks.LockGroup;

namespace ddge::exec::accessors {

inline namespace events {

export template <ddge::events::event_c Event_T>
class Reader {
public:
    using Event = Event_T;

    constexpr static auto lock_group() -> LockGroup;

    constexpr explicit Reader(
        const ddge::events::BufferedEventQueue<Event_T>& buffered_event_queue
    );

    [[nodiscard]]
    constexpr auto read() const -> std::span<const Event_T>;

private:
    std::reference_wrapper<const ddge::events::BufferedEventQueue<Event_T>>
        m_buffered_event_queue_ref;
};

}   // namespace events

}   // namespace ddge::exec::accessors

template <ddge::events::event_c Event_T>
constexpr auto ddge::exec::accessors::events::Reader<Event_T>::lock_group() -> LockGroup
{
    LockGroup lock_group;
    lock_group.expand<::ddge::exec::accessors::events::Event<Event>>(
        Lock{ CriticalSectionType::eShared }   //
    );
    lock_group.expand<EventManager>(Lock{ CriticalSectionType::eShared });
    return lock_group;
}

template <ddge::events::event_c Event_T>
constexpr ddge::exec::accessors::events::Reader<Event_T>::Reader(
    const ddge::events::BufferedEventQueue<Event_T>& buffered_event_queue
)
    : m_buffered_event_queue_ref{ buffered_event_queue }
{}

template <ddge::events::event_c Event_T>
constexpr auto ddge::exec::accessors::events::Reader<Event_T>::read() const
    -> std::span<const Event_T>
{
    return ddge::events::view(m_buffered_event_queue_ref.get());
}
