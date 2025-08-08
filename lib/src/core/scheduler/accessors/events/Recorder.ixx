module;

#include <concepts>
#include <functional>

export module core.scheduler.accessors.events.Recorder;

import core.events.event_c;
import core.events.BufferedEventQueue;
import core.events.EventManager;

import utility.meta.concepts.specialization_of;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.meta.type_traits.type_list.type_list_index_of;
import utility.meta.type_traits.type_list.type_list_front;
import utility.TypeList;

namespace core::scheduler::accessors {

inline namespace events {

export template <core::events::event_c... Events_T>
    requires(sizeof...(Events_T) != 0)
class Recorder {
public:
    using Events = util::TypeList<Events_T...>;

    constexpr explicit Recorder(
        core::events::BufferedEventQueue<Events_T>&... buffered_event_queues
    );

    template <typename... Args_T>
        requires(sizeof...(Events_T) == 1)
             && std::constructible_from<
                    util::meta::type_list_front_t<util::TypeList<Events_T...>>,
                    Args_T&&...>
    constexpr auto record(Args_T&&... args) const -> void;

    template <typename Event_T, typename... Args_T>
        requires(util::meta::type_list_contains_v<util::TypeList<Events_T...>, Event_T>)
             && std::constructible_from<Event_T, Args_T&&...>
    constexpr auto record(Args_T&&... args) const -> void;

private:
    std::tuple<std::reference_wrapper<core::events::BufferedEventQueue<Events_T>>...>
        m_buffered_event_queue_refs;
};

}   // namespace events

}   // namespace core::scheduler::accessors

template <core::events::event_c... Events_T>
    requires(sizeof...(Events_T) != 0)
constexpr core::scheduler::accessors::events::Recorder<Events_T...>::Recorder(
    core::events::BufferedEventQueue<Events_T>&... buffered_event_queues
)
    : m_buffered_event_queue_refs{ buffered_event_queues... }
{}

template <core::events::event_c... Events_T>
    requires(sizeof...(Events_T) != 0)
template <typename... Args_T>
    requires(sizeof...(Events_T) == 1)
         // TODO: use `Events_T...[0]` -
         // https://github.com/llvm/llvm-project/issues/138255
         && std::constructible_from<
                util::meta::type_list_front_t<util::TypeList<Events_T...>>,
                Args_T&&...>
constexpr auto core::scheduler::accessors::events::Recorder<Events_T...>::record(
    Args_T&&... args
) const -> void
{
    std::get<0>(m_buffered_event_queue_refs)
        .get()
        .emplace_back(std::forward<Args_T>(args)...);
}

template <core::events::event_c... Events_T>
    requires(sizeof...(Events_T) != 0)
template <typename Event_T, typename... Args_T>
    requires(util::meta::type_list_contains_v<util::TypeList<Events_T...>, Event_T>)
         && std::constructible_from<Event_T, Args_T&&...>
constexpr auto core::scheduler::accessors::events::Recorder<Events_T...>::record(
    Args_T&&... args
) const -> void
{
    std::get<util::meta::type_list_index_of_v<util::TypeList<Events_T...>, Event_T>>(
        m_buffered_event_queue_refs
    )
        .get()
        .emplace_back(std::forward<Args_T>(args)...);
}
