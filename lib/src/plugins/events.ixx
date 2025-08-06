module;

#include <flat_map>
#include <typeindex>
#include <utility>

#include "utility/contracts_macros.hpp"

export module plugins.events;

import addons.Events;

import app;

import core.events;

import utility.contracts;

namespace plugins {

export class Events {
public:
    template <core::events::event_c Event_T, typename Self_T>
    auto register_event(this Self_T&&) -> Self_T;

    template <app::decays_to_app_c App_T>
    [[nodiscard]]
    auto build(App_T&& app) && -> app::add_on_t<App_T, addons::Events>;

    template <core::events::event_c Event_T>
    [[nodiscard]]
    auto manages_event() const noexcept -> bool;

private:
    std::flat_map<std::type_index, core::events::ErasedBufferedEventQueue>
        m_buffered_event_queues;
};

}   // namespace plugins

template <core::events::event_c Event_T, typename Self_T>
auto plugins::Events::register_event(this Self_T&& self) -> Self_T
{
    Events& this_self{ static_cast<Events&>(self) };
    PRECOND((!this_self.manages_event<Event_T>()));
    this_self.m_buffered_event_queues
        .try_emplace(typeid(Event_T), std::in_place_type<Event_T>);
    return std::forward<Self_T>(self);
}

template <app::decays_to_app_c App_T>
auto plugins::Events::build(App_T&& app) && -> app::add_on_t<App_T, addons::Events>
{
    return std::forward<App_T>(app).add_on(
        addons::Events{ .event_manager{ std::move(m_buffered_event_queues) } }
    );
}

template <core::events::event_c Event_T>
auto plugins::Events::manages_event() const noexcept -> bool
{
    return m_buffered_event_queues.contains(typeid(Event_T));
}
