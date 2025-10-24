module;

#include <concepts>
#include <functional>

export module ddge.modules.exec.accessors.events:Recorder;

import :locks.ExclusiveEventLock;

import ddge.modules.events.event_c;
import ddge.modules.events.BufferedEventQueue;
import ddge.modules.events.EventManager;
import ddge.modules.exec.locks.Lockable;

import ddge.utility.meta.type_traits.type_list.type_list_contains;
import ddge.utility.meta.type_traits.type_list.type_list_index_of;
import ddge.utility.TypeList;

namespace ddge::exec::accessors {

inline namespace events {

export template <ddge::events::event_c... Events_T>
    requires(sizeof...(Events_T) != 0)
class Recorder : public Lockable<ExclusiveEventLock<Events_T>...> {
public:
    using Events = util::TypeList<Events_T...>;

    constexpr explicit Recorder(
        ddge::events::BufferedEventQueue<Events_T>&... buffered_event_queues
    );

    template <typename... Args_T>
        requires(sizeof...(Events_T) == 1)
             && std::constructible_from<Events_T...[0], Args_T&&...>
    constexpr auto record(Args_T&&... args) const -> void;

    template <typename Event_T, typename... Args_T>
        requires(util::meta::type_list_contains_v<util::TypeList<Events_T...>, Event_T>)
             && std::constructible_from<Event_T, Args_T&&...>
    constexpr auto record(Args_T&&... args) const -> void;

private:
    std::tuple<std::reference_wrapper<ddge::events::BufferedEventQueue<Events_T>>...>
        m_buffered_event_queue_refs;
};

}   // namespace events

}   // namespace ddge::exec::accessors

template <ddge::events::event_c... Events_T>
    requires(sizeof...(Events_T) != 0)
constexpr ddge::exec::accessors::events::Recorder<Events_T...>::Recorder(
    ddge::events::BufferedEventQueue<Events_T>&... buffered_event_queues
)
    : m_buffered_event_queue_refs{ buffered_event_queues... }
{}

template <ddge::events::event_c... Events_T>
    requires(sizeof...(Events_T) != 0)
template <typename... Args_T>
    requires(sizeof...(Events_T) == 1)
         && std::constructible_from<Events_T...[0], Args_T&&...>
constexpr auto ddge::exec::accessors::events::Recorder<Events_T...>::record(
    Args_T&&... args
) const -> void
{
    std::get<0>(m_buffered_event_queue_refs)
        .get()
        .emplace_back(std::forward<Args_T>(args)...);
}

template <ddge::events::event_c... Events_T>
    requires(sizeof...(Events_T) != 0)
template <typename Event_T, typename... Args_T>
    requires(ddge::util::meta::
                 type_list_contains_v<ddge::util::TypeList<Events_T...>, Event_T>)
         && std::constructible_from<Event_T, Args_T&&...>
constexpr auto ddge::exec::accessors::events::Recorder<Events_T...>::record(
    Args_T&&... args
) const -> void
{
    std::get<util::meta::type_list_index_of_v<util::TypeList<Events_T...>, Event_T>>(
        m_buffered_event_queue_refs
    )
        .get()
        .emplace_back(std::forward<Args_T>(args)...);
}
