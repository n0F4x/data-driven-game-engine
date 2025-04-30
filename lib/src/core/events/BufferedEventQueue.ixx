module;

#include <array>
#include <concepts>
#include <span>
#include <utility>
#include <vector>

export module core.events.BufferedEventQueue;

import core.events.decays_to_event_c;
import core.events.event_c;

import utility.meta.type_traits.const_like;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.TypeList;

namespace core::events {

export template <event_c Event_T>
class BufferedEventQueue;

export template <event_c Event_T>
[[nodiscard]]
constexpr auto view(const BufferedEventQueue<Event_T>& buffered_event_queue)
    -> std::span<const Event_T>;

export template <event_c Event_T>
class BufferedEventQueue {
public:
    template <typename... Args_T>
        requires std::constructible_from<Event_T, Args_T&&...>
    constexpr auto emplace_back(Args_T&&... args) -> void;

    constexpr auto swap_buffers() -> void;

    template <event_c UEvent_T>
    friend constexpr auto ::core::events::view(const BufferedEventQueue<UEvent_T>& self)
        -> std::span<const UEvent_T>;

private:
    using Queue = std::vector<Event_T>;

    uint8_t              m_recording_buffer_index{};
    std::array<Queue, 2> m_buffers;

    template <typename Self_T>
    [[nodiscard]]
    auto recording_buffer(this Self_T&) -> util::meta::const_like_t<Queue, Self_T>&;

    template <typename Self_T>
    [[nodiscard]]
    auto recorded_buffer(this Self_T&) -> util::meta::const_like_t<Queue, Self_T>&;

    [[nodiscard]]
    auto next_recording_buffer_index() const -> uint8_t;
};

}   // namespace core::events

template <core::events::event_c Event_T>
template <typename... Args_T>
    requires std::constructible_from<Event_T, Args_T&&...>
constexpr auto core::events::BufferedEventQueue<Event_T>::emplace_back(Args_T&&... args)
    -> void
{
    recording_buffer().emplace_back(std::forward<Args_T>(args)...);
}

template <core::events::event_c Event_T>
constexpr auto core::events::BufferedEventQueue<Event_T>::swap_buffers() -> void
{
    recorded_buffer().clear();
    m_recording_buffer_index = next_recording_buffer_index();
}

template <core::events::event_c Event_T>
constexpr auto core::events::view(
    const core::events::BufferedEventQueue<Event_T>& buffered_event_queue
) -> std::span<const Event_T>
{
    return buffered_event_queue.recorded_buffer();
}

template <core::events::event_c Event_T>
template <typename Self_T>
auto core::events::BufferedEventQueue<Event_T>::recording_buffer(this Self_T& self)
    -> util::meta::const_like_t<Queue, Self_T>&
{
    return self.m_buffers[self.m_recording_buffer_index];
}

template <core::events::event_c Event_T>
template <typename Self_T>
auto core::events::BufferedEventQueue<Event_T>::recorded_buffer(this Self_T& self)
    -> util::meta::const_like_t<Queue, Self_T>&
{
    return self.m_buffers[self.next_recording_buffer_index()];
}

template <core::events::event_c Event_T>
auto core::events::BufferedEventQueue<Event_T>::next_recording_buffer_index() const
    -> uint8_t
{
    return (m_recording_buffer_index + 1) % 2;
}
