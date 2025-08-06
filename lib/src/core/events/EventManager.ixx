module;

#include <flat_map>
#include <ranges>
#include <typeindex>
#include <utility>

#include "utility/contracts_macros.hpp"

export module core.events.EventManager;

import core.events.event_c;
import core.events.BufferedEventQueue;
import core.events.ErasedBufferedEventQueue;

import utility.containers.Any;
import utility.contracts;
import utility.meta.type_traits.const_like;

namespace core::events {

export class EventManager {
public:
    explicit EventManager(
        std::flat_map<std::type_index, ErasedBufferedEventQueue>&& buffered_event_queues
    );

    template <event_c Event_T, typename Self_T>
    [[nodiscard]]
    auto event_buffer(this Self_T&)
        -> util::meta::const_like_t<BufferedEventQueue<Event_T>, Self_T>&;

    template <event_c Event_T>
    [[nodiscard]]
    auto manages_event() const noexcept -> bool;

    auto process_events() -> void;

private:
    std::flat_map<std::type_index, ErasedBufferedEventQueue> m_buffered_event_queues;
};

}   // namespace core::events

template <core::events::event_c Event_T, typename Self_T>
auto core::events::EventManager::event_buffer(this Self_T& self)
    -> util::meta::const_like_t<BufferedEventQueue<Event_T>, Self_T>&
{
    const auto iter{ self.m_buffered_event_queues.find(typeid(Event_T)) };
    PRECOND(iter != self.m_buffered_event_queues.cend());
    return util::any_cast<BufferedEventQueue<Event_T>>(iter->second);
}

template <core::events::event_c Event_T>
auto core::events::EventManager::manages_event() const noexcept -> bool
{
    return m_buffered_event_queues.contains(typeid(Event_T));
}

module :private;

core::events::EventManager::EventManager(
    std::flat_map<std::type_index, ErasedBufferedEventQueue>&& buffered_event_queues
)
    : m_buffered_event_queues{ std::move(buffered_event_queues) }
{}

auto core::events::EventManager::process_events() -> void
{
    for (auto& buffered_event_queue : m_buffered_event_queues | std::views::values) {
        buffered_event_queue.swap_buffers();
    }
}
