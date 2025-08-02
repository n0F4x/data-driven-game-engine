module;

#include <concepts>
#include <functional>
#include <type_traits>

export module extensions.scheduler.providers.EventProvider;

import app;

import core.events.EventManager;
import core.events.BufferedEventQueue;

import extensions.scheduler.accessors.events;

import utility.meta.concepts.decays_to;
import utility.meta.concepts.specialization_of;
import utility.meta.concepts.type_list.type_list_all_of;
import utility.meta.type_traits.type_list.type_list_transform;
import utility.meta.type_traits.underlying;

namespace extensions::scheduler::providers {

export template <
    util::meta::specialization_of_c<core::events::EventManager> EventManager_T,
    typename EventsAddon_T>
class EventProvider {
    template <typename Event_T>
    struct IsRegistered
        : std::bool_constant<EventManager_T::template registered<Event_T>()> {};

    template <typename Accessor_T>
    constexpr static bool all_registered = util::meta::type_list_all_of_c<
        util::meta::type_list_transform_t<Accessor_T, std::remove_cvref>,
        IsRegistered>;

public:
    template <app::has_addons_c<EventsAddon_T> App_T>
    constexpr explicit EventProvider(App_T& app);

    template <util::meta::decays_to_c<accessors::events::Processor> Accessor_T>
    [[nodiscard]]
    auto provide() const -> accessors::events::Processor;

    template <typename Accessor_T>
        requires util::meta::specialization_of_c<
            std::remove_cvref_t<Accessor_T>,
            accessors::events::Recorder>   //
    [[nodiscard]]
    constexpr auto provide() const -> std::remove_cvref_t<Accessor_T>
        requires(all_registered<std::remove_cvref_t<Accessor_T>>);

    template <typename Accessor_T>
        requires util::meta::specialization_of_c<
            std::remove_cvref_t<Accessor_T>,
            accessors::events::Reader>   //
    [[nodiscard]]
    constexpr auto provide() const -> std::remove_cvref_t<Accessor_T>
        requires(all_registered<std::remove_cvref_t<Accessor_T>>);

private:
    std::reference_wrapper<EventManager_T> m_event_manager_ref;
};

}   // namespace extensions::scheduler::providers

template <
    util::meta::specialization_of_c<core::events::EventManager> EventManager_T,
    typename EventsAddon_T>
template <app::has_addons_c<EventsAddon_T> App_T>
constexpr extensions::scheduler::providers::EventProvider<EventManager_T, EventsAddon_T>::
    EventProvider(App_T& app)
    : m_event_manager_ref{ app.event_manager }
{}

template <
    util::meta::specialization_of_c<core::events::EventManager> EventManager_T,
    typename EventsAddon_T>
template <util::meta::decays_to_c<extensions::scheduler::accessors::events::Processor>
              Accessor_T>
auto extensions::scheduler::providers::EventProvider<EventManager_T, EventsAddon_T>::
    provide() const -> extensions::scheduler::accessors::events::Processor
{
    return std::remove_cvref_t<Accessor_T>{ m_event_manager_ref.get() };
}

template <
    util::meta::specialization_of_c<core::events::EventManager> EventManager_T,
    typename EventsAddon_T>
template <typename Accessor_T>
    requires util::meta::specialization_of_c<
        std::remove_cvref_t<Accessor_T>,
        extensions::scheduler::accessors::events::Recorder>
constexpr auto extensions::scheduler::providers::
    EventProvider<EventManager_T, EventsAddon_T>::provide() const
    -> std::remove_cvref_t<Accessor_T>
    requires(all_registered<std::remove_cvref_t<Accessor_T>>)
{
    return std::remove_cvref_t<Accessor_T>{ m_event_manager_ref.get() };
}

template <
    util::meta::specialization_of_c<core::events::EventManager> EventManager_T,
    typename EventsAddon_T>
template <typename Accessor_T>
    requires util::meta::specialization_of_c<
        std::remove_cvref_t<Accessor_T>,
        extensions::scheduler::accessors::events::Reader>
constexpr auto extensions::scheduler::providers::
    EventProvider<EventManager_T, EventsAddon_T>::provide() const
    -> std::remove_cvref_t<Accessor_T>
    requires(all_registered<std::remove_cvref_t<Accessor_T>>)
{
    return std::remove_cvref_t<Accessor_T>{
        m_event_manager_ref.get()
            .template event_buffer<
                util::meta::underlying_t<std::remove_cvref_t<Accessor_T>>>()
    };
}
