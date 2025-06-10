module;

#include <concepts>
#include <functional>
#include <type_traits>

export module extensions.scheduler.argument_providers.events.EventProvider;

import core.events.EventManager;
import core.events.BufferedEventQueue;

import extensions.scheduler.accessors.events;

import utility.meta.concepts.decays_to;
import utility.meta.concepts.specialization_of;
import utility.meta.concepts.type_list.type_list_all_of;
import utility.meta.type_traits.type_list.type_list_transform;
import utility.meta.type_traits.underlying;

namespace extensions::scheduler::argument_providers {

export template <util::meta::specialization_of_c<core::events::EventManager> EventManager_T>
class EventProvider {
    template <typename Event_T>
    struct IsRegistered
        : std::bool_constant<EventManager_T::template registered<Event_T>()> {};

    template <typename Accessor_T>
    constexpr static bool all_registered = util::meta::type_list_all_of_c<
        util::meta::type_list_transform_t<Accessor_T, std::remove_cvref>,
        IsRegistered>;

public:
    constexpr explicit EventProvider(EventManager_T& resource_manager);

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

}   // namespace extensions::scheduler::argument_providers

template <util::meta::specialization_of_c<core::events::EventManager> EventManager_T>
constexpr extensions::scheduler::argument_providers::EventProvider<EventManager_T>::
    EventProvider(EventManager_T& resource_manager)
    : m_event_manager_ref{ resource_manager }
{}

template <util::meta::specialization_of_c<core::events::EventManager> EventManager_T>
template <util::meta::decays_to_c<extensions::scheduler::accessors::events::Processor>
              Accessor_T>
auto extensions::scheduler::argument_providers::EventProvider<EventManager_T>::provide(
) const -> extensions::scheduler::accessors::events::Processor
{
    return std::remove_cvref_t<Accessor_T>{ m_event_manager_ref.get() };
}

template <util::meta::specialization_of_c<core::events::EventManager> EventManager_T>
template <typename Accessor_T>
    requires util::meta::specialization_of_c<
        std::remove_cvref_t<Accessor_T>,
        extensions::scheduler::accessors::events::Recorder>
constexpr auto
    extensions::scheduler::argument_providers::EventProvider<EventManager_T>::provide(
    ) const -> std::remove_cvref_t<Accessor_T>
    requires(all_registered<std::remove_cvref_t<Accessor_T>>)
{
    return std::remove_cvref_t<Accessor_T>{ m_event_manager_ref.get() };
}

template <util::meta::specialization_of_c<core::events::EventManager> EventManager_T>
template <typename Accessor_T>
    requires util::meta::specialization_of_c<
        std::remove_cvref_t<Accessor_T>,
        extensions::scheduler::accessors::events::Reader>
constexpr auto
    extensions::scheduler::argument_providers::EventProvider<EventManager_T>::provide(
    ) const -> std::remove_cvref_t<Accessor_T>
    requires(all_registered<std::remove_cvref_t<Accessor_T>>)
{
    return std::remove_cvref_t<Accessor_T>{
        m_event_manager_ref.get()
            .template event_buffer<
                util::meta::underlying_t<std::remove_cvref_t<Accessor_T>>>()
    };
}
