module;

#include <concepts>
#include <format>
#include <functional>
#include <type_traits>

#include "utility/contracts_macros.hpp"

export module ddge.modules.scheduler.providers.EventProvider;

import ddge.modules.app;

import ddge.modules.events;
import ddge.modules.scheduler.ProviderFor;
import ddge.modules.store.Store;

import ddge.modules.scheduler.accessors.events;
import ddge.modules.scheduler.ProviderOf;

import ddge.utility.contracts;
import ddge.utility.meta.algorithms.apply;
import ddge.utility.meta.algorithms.for_each;
import ddge.utility.meta.concepts.specialization_of;
import ddge.utility.meta.reflection.name_of;

namespace ddge::scheduler::providers {

class EventProvider {
public:
    template <ddge::app::has_addons_c<events::Addon> App_T>
    constexpr explicit EventProvider(App_T& app);

    template <std::same_as<accessors::events::Processor>>
    [[nodiscard]]
    auto provide() const -> accessors::events::Processor;

    template <util::meta::specialization_of_c<accessors::events::Recorder> Recorder_T>
    [[nodiscard]]
    constexpr auto provide() const -> Recorder_T;

    template <util::meta::specialization_of_c<accessors::events::Reader> Reader_T>
    [[nodiscard]]
    constexpr auto provide() const -> Reader_T;

private:
    std::reference_wrapper<ddge::events::EventManager> m_event_manager_ref;
};

}   // namespace ddge::scheduler::providers

template <>
struct ddge::scheduler::ProviderOf<ddge::events::Addon>
    : std::type_identity<ddge::scheduler::providers::EventProvider> {};

template <>
struct ddge::scheduler::ProviderFor<ddge::scheduler::accessors::events::Processor>
    : std::type_identity<ddge::scheduler::providers::EventProvider> {};

template <typename... Events_T>
struct ddge::scheduler::
    ProviderFor<ddge::scheduler::accessors::events::Recorder<Events_T...>>
    : std::type_identity<ddge::scheduler::providers::EventProvider> {};

template <typename Event_T>
struct ddge::scheduler::ProviderFor<ddge::scheduler::accessors::events::Reader<Event_T>>
    : std::type_identity<ddge::scheduler::providers::EventProvider> {};

template <ddge::app::has_addons_c<ddge::events::Addon> App_T>
constexpr ddge::scheduler::providers::EventProvider::EventProvider(App_T& app)
    : m_event_manager_ref{ app.event_manager }
{}

template <std::same_as<ddge::scheduler::accessors::events::Processor> Accessor_T>
auto ddge::scheduler::providers::EventProvider::provide() const
    -> ddge::scheduler::accessors::events::Processor
{
    return ddge::scheduler::accessors::events::Processor{ m_event_manager_ref };
}

template <ddge::util::meta::
              specialization_of_c<ddge::scheduler::accessors::events::Recorder> Recorder_T>
constexpr auto ddge::scheduler::providers::EventProvider::provide() const -> Recorder_T
{
    using Events = typename Recorder_T::Events;

    return util::meta::apply<Events>([this]<typename... Events_T> -> Recorder_T {
        PRECOND((m_event_manager_ref.get().manages_event<Events_T>() && ...));

        return Recorder_T{ m_event_manager_ref.get().event_buffer<Events_T>()... };
    });
}

template <ddge::util::meta::
              specialization_of_c<ddge::scheduler::accessors::events::Reader> Reader_T>
constexpr auto ddge::scheduler::providers::EventProvider::provide() const -> Reader_T
{
    using Event = typename Reader_T::Event;

    PRECOND(
        (m_event_manager_ref.get().manages_event<Event>()),
        std::format("Event {} was not registered", util::meta::name_of<Event>())
    );

    return Reader_T{ m_event_manager_ref.get().event_buffer<Event>() };
}
