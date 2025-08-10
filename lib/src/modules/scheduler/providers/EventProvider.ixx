module;

#include <concepts>
#include <format>
#include <functional>
#include <type_traits>

#include "utility/contracts_macros.hpp"

export module modules.scheduler.providers.EventProvider;

import modules.app;

import modules.events;
import modules.scheduler.ProviderFor;
import modules.store.Store;

import modules.scheduler.accessors.events;
import modules.scheduler.ProviderOf;

import utility.contracts;
import utility.meta.algorithms.apply;
import utility.meta.algorithms.for_each;
import utility.meta.concepts.specialization_of;
import utility.meta.reflection.name_of;

namespace modules::scheduler::providers {

class EventProvider {
public:
    template <modules::app::has_addons_c<events::Addon> App_T>
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
    std::reference_wrapper<modules::events::EventManager> m_event_manager_ref;
};

}   // namespace modules::scheduler::providers

template <>
struct modules::scheduler::ProviderOf<modules::events::Addon>
    : std::type_identity<modules::scheduler::providers::EventProvider> {};

template <>
struct modules::scheduler::ProviderFor<modules::scheduler::accessors::events::Processor>
    : std::type_identity<modules::scheduler::providers::EventProvider> {};

template <typename... Events_T>
struct modules::scheduler::
    ProviderFor<modules::scheduler::accessors::events::Recorder<Events_T...>>
    : std::type_identity<modules::scheduler::providers::EventProvider> {};

template <typename Event_T>
struct modules::scheduler::
    ProviderFor<modules::scheduler::accessors::events::Reader<Event_T>>
    : std::type_identity<modules::scheduler::providers::EventProvider> {};

template <modules::app::has_addons_c<modules::events::Addon> App_T>
constexpr modules::scheduler::providers::EventProvider::EventProvider(App_T& app)
    : m_event_manager_ref{ app.event_manager }
{}

template <std::same_as<modules::scheduler::accessors::events::Processor> Accessor_T>
auto modules::scheduler::providers::EventProvider::provide() const
    -> modules::scheduler::accessors::events::Processor
{
    return modules::scheduler::accessors::events::Processor{ m_event_manager_ref };
}

template <util::meta::specialization_of_c<modules::scheduler::accessors::events::Recorder>
              Recorder_T>
constexpr auto modules::scheduler::providers::EventProvider::provide() const -> Recorder_T
{
    using Events = typename Recorder_T::Events;

    return util::meta::apply<Events>([this]<typename... Events_T> -> Recorder_T {
        PRECOND((m_event_manager_ref.get().manages_event<Events_T>() && ...));

        return Recorder_T{ m_event_manager_ref.get().event_buffer<Events_T>()... };
    });
}

template <util::meta::specialization_of_c<modules::scheduler::accessors::events::Reader>
              Reader_T>
constexpr auto modules::scheduler::providers::EventProvider::provide() const -> Reader_T
{
    using Event = typename Reader_T::Event;

    PRECOND(
        (m_event_manager_ref.get().manages_event<Event>()),
        std::format("Event {} was not registered", util::meta::name_of<Event>())
    );

    return Reader_T{ m_event_manager_ref.get().event_buffer<Event>() };
}
