module;

#include <concepts>
#include <span>
#include <utility>
#include <vector>

export module core.events.EventQueue;

import core.events.decays_to_event_c;
import core.events.event_c;

import utility.meta.type_traits.type_list.type_list_contains;
import utility.TypeList;

namespace core::events {

export template <event_c Event_T>
class EventQueue;

export template <event_c Event_T>
[[nodiscard]]
constexpr auto view(const EventQueue<Event_T>& event_queue) -> std::span<const Event_T>;

export template <event_c Event_T>
class EventQueue {
public:
    template <typename... Args_T>
        requires std::constructible_from<Event_T, Args_T&&...>
    constexpr auto emplace_back(Args_T&&... args) -> void;

    constexpr auto reset() -> void;

    template <event_c UEvent_T>
    friend constexpr auto ::core::events::view(const EventQueue<UEvent_T>& self)
        -> std::span<const UEvent_T>;

private:
    std::vector<Event_T> m_container;
};

}   // namespace core::events

template <core::events::event_c Event_T>
constexpr auto core::events::view(const core::events::EventQueue<Event_T>& event_queue)
    -> std::span<const Event_T>
{
    return event_queue.m_container;
}

template <core::events::event_c Event_T>
template <typename... Args_T>
    requires std::constructible_from<Event_T, Args_T&&...>
constexpr auto core::events::EventQueue<Event_T>::emplace_back(Args_T&&... args) -> void
{
    m_container.emplace_back(std::forward<Args_T>(args)...);
}

template <core::events::event_c Event_T>
constexpr auto core::events::EventQueue<Event_T>::reset() -> void
{
    m_container.clear();
}
