module;

#include <functional>
#include <span>

export module ddge.modules.scheduler.accessors.events.Reader;

import ddge.modules.events.event_c;
import ddge.modules.events.BufferedEventQueue;
import ddge.modules.scheduler.accessors.events.Event;
import ddge.modules.scheduler.accessors.events.EventManager;
import ddge.modules.scheduler.locks.CriticalSectionType;
import ddge.modules.scheduler.locks.Lock;
import ddge.modules.scheduler.locks.LockGroup;

namespace ddge::scheduler::accessors {

inline namespace events {

export template <ddge::events::event_c Event_T>
class Reader {
public:
    using Event = Event_T;

    constexpr static auto lock_group() -> const LockGroup&;

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

}   // namespace ddge::scheduler::accessors

template <ddge::events::event_c Event_T>
constexpr auto ddge::scheduler::accessors::events::Reader<Event_T>::lock_group()
    -> const LockGroup&
{
    static const LockGroup lock_group{ [] -> LockGroup {
        LockGroup          result;
        result.expand<::ddge::scheduler::accessors::events::Event<Event>>(
            Lock{ CriticalSectionType::eShared }   //
        );
        result.expand<EventManager>(Lock{ CriticalSectionType::eShared });
        return result;
    }() };

    return lock_group;
}

template <ddge::events::event_c Event_T>
constexpr ddge::scheduler::accessors::events::Reader<Event_T>::Reader(
    const ddge::events::BufferedEventQueue<Event_T>& buffered_event_queue
)
    : m_buffered_event_queue_ref{ buffered_event_queue }
{}

template <ddge::events::event_c Event_T>
constexpr auto ddge::scheduler::accessors::events::Reader<Event_T>::read() const
    -> std::span<const Event_T>
{
    return ddge::events::view(m_buffered_event_queue_ref.get());
}
