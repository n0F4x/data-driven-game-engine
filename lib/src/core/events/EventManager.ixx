module;

#include <concepts>
#include <cstdint>
#include <span>
#include <tuple>
#include <utility>

export module core.events.EventManager;

import utility.meta.algorithms.for_each;
import utility.meta.type_traits.const_like;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.TypeList;

import core.events.decays_to_event_c;
import core.events.event_c;
import core.events.EventQueue;

namespace core::events {

export template <event_c... Events_T>
class EventManager {
public:
    template <event_c Event_T, typename Self_T>
        requires util::meta::type_list_contains_v<util::TypeList<Events_T...>, Event_T>
    [[nodiscard]]
    auto get(this Self_T&) -> std::span<const Event_T>;

    template <event_c Event_T, typename Self_T>
        requires util::meta::type_list_contains_v<util::TypeList<Events_T...>, Event_T>
    [[nodiscard]]
    auto recorder_queue_for(this Self_T&)
        -> util::meta::const_like_t<EventQueue<Event_T>, Self_T>&;

    template <decays_to_event_c Event_T>
        requires util::meta::
                     type_list_contains_v<util::TypeList<Events_T...>, std::decay_t<Event_T>>
              && std::constructible_from<std::decay_t<Event_T>, Event_T>
    auto record(Event_T& event) -> void;

    template <event_c Event_T, typename... Args_T>
        requires util::meta::type_list_contains_v<util::TypeList<Events_T...>, Event_T>
              && std::constructible_from<Event_T, Args_T&&...>
    auto record(std::in_place_type_t<Event_T>, Args_T&&... args) -> void;

    auto clear_events() -> void;

private:
    constexpr static uint8_t buffer_size{ 2 };

    uint8_t                                                      m_current_buffer_index{};
    std::array<std::tuple<EventQueue<Events_T>...>, buffer_size> m_event_buffers;

    [[nodiscard]]
    auto next_buffer_index() const -> uint8_t;
};

}   // namespace core::events

template <core::events::event_c... Events_T>
template <core::events::event_c Event_T, typename Self_T>
    requires util::meta::type_list_contains_v<util::TypeList<Events_T...>, Event_T>
auto core::events::EventManager<Events_T...>::get(this Self_T& self)
    -> std::span<const Event_T>
{
    return core::events::view(
        std::get<EventQueue<Event_T>>(self.m_event_buffers.at(self.m_current_buffer_index))
    );
}

template <core::events::event_c... Events_T>
template <core::events::event_c Event_T, typename Self_T>
    requires util::meta::type_list_contains_v<util::TypeList<Events_T...>, Event_T>
auto core::events::EventManager<Events_T...>::recorder_queue_for(this Self_T& self)
    -> util::meta::const_like_t<EventQueue<Event_T>, Self_T>&
{
    return std::get<EventQueue<Event_T>>(self.m_event_buffers.at(self.next_buffer_index())
    );
}

template <core::events::event_c... Events_T>
template <core::events::decays_to_event_c Event_T>
    requires util::meta::
                 type_list_contains_v<util::TypeList<Events_T...>, std::decay_t<Event_T>>
          && std::constructible_from<std::decay_t<Event_T>, Event_T>
auto core::events::EventManager<Events_T...>::record(Event_T& event) -> void
{
    std::get<EventQueue<std::decay_t<Event_T>>>(m_event_buffers.at(next_buffer_index()))
        .push_back(std::forward<Event_T>(event));
}

template <core::events::event_c... Events_T>
template <core::events::event_c Event_T, typename... Args_T>
    requires util::meta::type_list_contains_v<util::TypeList<Events_T...>, Event_T>
          && std::constructible_from<Event_T, Args_T&&...>
auto core::events::EventManager<Events_T...>::record(
    std::in_place_type_t<Event_T>,
    Args_T&&... args
) -> void
{
    return std::get<EventQueue<Event_T>>(m_event_buffers.at(next_buffer_index()))
        .emplace_back(std::forward<Args_T>(args)...);
}

template <core::events::event_c... Events_T>
auto core::events::EventManager<Events_T...>::clear_events() -> void
{
    util::meta::for_each<std::make_index_sequence<sizeof...(Events_T)>>(
        [this]<size_t index_T> {
            std::get<index_T>(m_event_buffers.at(m_current_buffer_index)).reset();
        }
    );

    m_current_buffer_index = next_buffer_index();
}

template <core::events::event_c... Events_T>
auto core::events::EventManager<Events_T...>::next_buffer_index() const -> uint8_t
{
    return (m_current_buffer_index + 1) % buffer_size;
}
