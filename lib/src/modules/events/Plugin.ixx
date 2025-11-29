module;

#include <flat_map>
#include <typeindex>
#include <utility>

#include "utility/contracts_macros.hpp"

export module ddge.modules.events.Plugin;

import ddge.modules.app;
import ddge.modules.events.Addon;
import ddge.modules.events.event_c;
import ddge.modules.events.ErasedBufferedEventQueue;

import ddge.utility.contracts;

namespace ddge::events {

export class Plugin {
public:
    template <ddge::events::event_c Event_T, typename Self_T>
    auto register_event(this Self_T&&) -> Self_T;

    template <ddge::app::decays_to_app_c App_T>
    [[nodiscard]]
    auto build(App_T&& app) && -> ddge::app::add_on_t<App_T, Addon>;

    template <ddge::events::event_c Event_T>
    [[nodiscard]]
    auto manages_event() const noexcept -> bool;

private:
    std::flat_map<std::type_index, ddge::events::ErasedBufferedEventQueue>
        m_buffered_event_queues;
};

}   // namespace ddge::events

template <ddge::events::event_c Event_T, typename Self_T>
auto ddge::events::Plugin::register_event(this Self_T&& self) -> Self_T
{
    Plugin& this_self{ static_cast<Plugin&>(self) };
    PRECOND((!this_self.manages_event<Event_T>()));
    this_self.m_buffered_event_queues.try_emplace(
        typeid(Event_T), std::in_place_type<Event_T>
    );
    return std::forward<Self_T>(self);
}

template <ddge::app::decays_to_app_c App_T>
auto ddge::events::Plugin::build(App_T&& app) && -> ddge::app::add_on_t<App_T, Addon>
{
    return std::forward<App_T>(app).add_on(
        Addon{ .event_manager{ std::move(m_buffered_event_queues) } }
    );
}

template <ddge::events::event_c Event_T>
auto ddge::events::Plugin::manages_event() const noexcept -> bool
{
    return m_buffered_event_queues.contains(typeid(Event_T));
}
