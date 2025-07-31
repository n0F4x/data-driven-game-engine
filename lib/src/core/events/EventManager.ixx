module;

#include <cstdint>
#include <tuple>
#include <type_traits>

export module core.events.EventManager;

import utility.meta.algorithms.for_each;
import utility.meta.type_traits.const_like;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.TypeList;

import core.events.event_c;
import core.events.BufferedEventQueue;

namespace core::events {

export template <event_c... Events_T>
class EventManager {
public:
    template <event_c Event_T>
    constexpr static std::bool_constant<
        util::meta::type_list_contains_v<util::TypeList<Events_T...>, Event_T>>
        registered;

    template <event_c Event_T, typename Self_T>
    [[nodiscard]]
    auto event_buffer(this Self_T&)
        -> util::meta::const_like_t<BufferedEventQueue<Event_T>, Self_T>&   //
        requires(registered<Event_T>());

    auto process_events() -> void;

private:
    std::tuple<BufferedEventQueue<Events_T>...> m_event_buffers;
};

}   // namespace core::events

template <core::events::event_c... Events_T>
template <core::events::event_c Event_T, typename Self_T>
auto core::events::EventManager<Events_T...>::event_buffer(this Self_T& self)
    -> util::meta::const_like_t<BufferedEventQueue<Event_T>, Self_T>&   //
    requires(registered<Event_T>())                                     //
{                                                                       //
    return std::get<BufferedEventQueue<Event_T>>(self.m_event_buffers);
}

template <core::events::event_c... Events_T>
auto core::events::EventManager<Events_T...>::process_events() -> void
{
    util::meta::for_each<std::make_index_sequence<sizeof...(Events_T)>>(
        [this]<std::size_t index_T> { std::get<index_T>(m_event_buffers).swap_buffers(); }
    );
}
