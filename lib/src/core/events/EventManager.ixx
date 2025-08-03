module;

#include <functional>

export module core.events.EventManager;

import core.events.event_c;
import core.events.BufferedEventQueue;
import core.store.Store;

import utility.meta.algorithms.for_each;
import utility.meta.type_traits.const_like;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.tuple.tuple_for_each;
import utility.TypeList;

namespace core::events {

export class EventManager {
public:
    template <event_c... Events_T>
    explicit EventManager(util::TypeList<Events_T...>);

    template <event_c Event_T, typename Self_T>
    [[nodiscard]]
    auto event_buffer(this Self_T&)
        -> util::meta::const_like_t<BufferedEventQueue<Event_T>, Self_T>&;

    template <event_c Event_T>
    [[nodiscard]]
    auto manages_event() const noexcept -> bool;

    auto process_events() -> void;

private:
    store::Store          m_event_buffers;
    std::function<void()> m_process_events;
};

}   // namespace core::events

template <core::events::event_c... Events_T>
core::events::EventManager::EventManager(util::TypeList<Events_T...>)
{
    (m_event_buffers.emplace<BufferedEventQueue<Events_T>>(), ...);
    m_process_events = [buffer_refs_tuple = std::
                            tuple<std::reference_wrapper<BufferedEventQueue<Events_T>>...>{
                                m_event_buffers.at<BufferedEventQueue<Events_T>>()... }] {
        (std::get<std::reference_wrapper<BufferedEventQueue<Events_T>>>(buffer_refs_tuple)
             .get()
             .swap_buffers(),
         ...);
    };
}

template <core::events::event_c Event_T, typename Self_T>
auto core::events::EventManager::event_buffer(this Self_T& self)
    -> util::meta::const_like_t<BufferedEventQueue<Event_T>, Self_T>&
{
    return self.m_event_buffers.template at<BufferedEventQueue<Event_T>>();
}

template <core::events::event_c Event_T>
auto core::events::EventManager::manages_event() const noexcept -> bool
{
    return m_event_buffers.contains<BufferedEventQueue<Event_T>>();
}

module :private;

auto core::events::EventManager::process_events() -> void
{
    m_process_events();
}
