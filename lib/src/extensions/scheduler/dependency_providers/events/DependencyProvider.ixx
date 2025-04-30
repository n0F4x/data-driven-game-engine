module;

#include <concepts>
#include <functional>

export module extensions.scheduler.dependency_providers.events.DependencyProvider;

import core.events.EventManager;
import core.events.BufferedEventQueue;

import extensions.scheduler.accessors.events;

import utility.meta.concepts.specialization_of;
import utility.meta.type_traits.underlying;

namespace extensions::scheduler::dependency_providers::events {

export template <util::meta::specialization_of_c<core::events::EventManager> EventManager_T>
class DependencyProvider {
public:
    constexpr explicit DependencyProvider(EventManager_T& resource_manager);

    template <typename EventTag_T>
        requires std::same_as<std::remove_const_t<EventTag_T>, accessors::events::Processor>
    [[nodiscard]]
    auto provide() const -> EventTag_T;

    template <util::meta::specialization_of_c<accessors::events::Recorder> EventTag_T>
    [[nodiscard]]
    constexpr auto provide() const -> EventTag_T;

    template <util::meta::specialization_of_c<accessors::events::Reader> EventTag_T>
    [[nodiscard]]
    constexpr auto provide() const -> EventTag_T;

private:
    std::reference_wrapper<EventManager_T> m_event_manager_ref;
};

}   // namespace extensions::scheduler::dependency_providers::events

template <util::meta::specialization_of_c<core::events::EventManager> EventManager_T>
constexpr extensions::scheduler::dependency_providers::events::
    DependencyProvider<EventManager_T>::DependencyProvider(
        EventManager_T& resource_manager
    )
    : m_event_manager_ref{ resource_manager }
{}

template <util::meta::specialization_of_c<core::events::EventManager> EventManager_T>
template <typename EventTag_T>
    requires std::same_as<
        std::remove_const_t<EventTag_T>,
        extensions::scheduler::accessors::events::Processor>
auto extensions::scheduler::dependency_providers::events::
    DependencyProvider<EventManager_T>::provide() const -> EventTag_T
{
    return EventTag_T{ m_event_manager_ref.get() };
}

template <util::meta::specialization_of_c<core::events::EventManager> EventManager_T>
template <util::meta::specialization_of_c<
    extensions::scheduler::accessors::events::Recorder> EventTag_T>
constexpr auto extensions::scheduler::dependency_providers::events::
    DependencyProvider<EventManager_T>::provide() const -> EventTag_T
{
    return EventTag_T{ m_event_manager_ref.get()
                           .template event_buffer<util::meta::underlying_t<EventTag_T>>(
                           ) };
}

template <util::meta::specialization_of_c<core::events::EventManager> EventManager_T>
template <util::meta::specialization_of_c<extensions::scheduler::accessors::events::Reader>
              EventTag_T>
constexpr auto extensions::scheduler::dependency_providers::events::
    DependencyProvider<EventManager_T>::provide() const -> EventTag_T
{
    return EventTag_T{ m_event_manager_ref.get()
                           .template event_buffer<util::meta::underlying_t<EventTag_T>>(
                           ) };
}
