export module addons.Events;

import core.events;

import extensions.scheduler.argument_provider_for;
import extensions.scheduler.argument_providers.EventProvider;

namespace addons {

export struct EventsTag {};

export template <core::events::event_c... Events_T>
struct Events : EventsTag {
    core::events::EventManager<Events_T...> event_manager;
};

}   // namespace addons

template <core::events::event_c... Events_T>
struct extensions::scheduler::ArgumentProviderFor<addons::Events<Events_T...>> {
    using type = extensions::scheduler::argument_providers::
        EventProvider<core::events::EventManager<Events_T...>, addons::Events<Events_T...>>;
};
