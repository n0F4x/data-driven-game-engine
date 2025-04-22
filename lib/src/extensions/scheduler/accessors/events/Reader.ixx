module;

#include <span>
#include <type_traits>

export module extensions.scheduler.accessors.events.Reader;

import core.events.event_c;

namespace extensions::scheduler::accessors::events {

export template <typename Event_T>
    requires core::events::event_c<std::remove_const_t<Event_T>>
class Reader {
    using UnderlyingView = std::span<const Event_T>;

    using Iterator        = typename UnderlyingView::iterator;
    using ReverseIterator = typename UnderlyingView::reverse_iterator;

    // TODO: p2278r4
    // using ConstIterator        = typename UnderlyingView::const_iterator;
    // using ConstReverseIterator = typename UnderlyingView::const_reverse_iterator;

public:
    constexpr explicit Reader(UnderlyingView events);

    [[nodiscard]]
    constexpr auto count() const -> size_t;

    [[nodiscard]]
    constexpr auto front() const -> Event_T&;
    [[nodiscard]]
    constexpr auto back() const -> Event_T&;

    [[nodiscard]]
    constexpr auto begin() const -> Iterator;
    [[nodiscard]]
    constexpr auto end() const -> Iterator;

    [[nodiscard]]
    constexpr auto rbegin() const -> Iterator;
    [[nodiscard]]
    constexpr auto rend() const -> Iterator;

private:
    UnderlyingView m_events_view;
};

}   // namespace extensions::scheduler::accessors::events

template <typename Event_T>
    requires core::events::event_c<std::remove_const_t<Event_T>>
constexpr extensions::scheduler::accessors::events::
    Reader<Event_T>::Reader(std::span<const Event_T> events)
    : m_events_view{ events }
{}

template <typename Event_T>
    requires core::events::event_c<std::remove_const_t<Event_T>>
constexpr auto extensions::scheduler::accessors::events::
    Reader<Event_T>::count() const -> size_t
{
    return m_events_view.size();
}

template <typename Event_T>
    requires core::events::event_c<std::remove_const_t<Event_T>>
constexpr auto extensions::scheduler::accessors::events::
    Reader<Event_T>::front() const -> Event_T&
{
    return m_events_view.front();
}

template <typename Event_T>
    requires core::events::event_c<std::remove_const_t<Event_T>>
constexpr auto extensions::scheduler::accessors::events::
    Reader<Event_T>::back() const -> Event_T&
{
    return m_events_view.back();
}

template <typename Event_T>
    requires core::events::event_c<std::remove_const_t<Event_T>>
constexpr auto extensions::scheduler::accessors::events::
    Reader<Event_T>::begin() const -> Iterator
{
    return m_events_view.begin();
}

template <typename Event_T>
    requires core::events::event_c<std::remove_const_t<Event_T>>
constexpr auto extensions::scheduler::accessors::events::
    Reader<Event_T>::end() const -> Iterator
{
    return m_events_view.end();
}

template <typename Event_T>
    requires core::events::event_c<std::remove_const_t<Event_T>>
constexpr auto extensions::scheduler::accessors::events::
    Reader<Event_T>::rbegin() const -> Iterator
{
    return m_events_view.rbegin();
}

template <typename Event_T>
    requires core::events::event_c<std::remove_const_t<Event_T>>
constexpr auto extensions::scheduler::accessors::events::
    Reader<Event_T>::rend() const -> Iterator
{
    return m_events_view.rend();
}
