export module extensions.scheduler.argument_providers.events;

import addons.Events;

import app.has_addons_c;

export import extensions.scheduler.argument_providers.events.EventProvider;

namespace extensions::scheduler::argument_providers {

export struct EventProviderClosure {
    [[nodiscard]]
    constexpr static auto
        operator()(app::has_addons_c<addons::EventsTag> auto&);
};

export constexpr EventProviderClosure event_provider;

}   // namespace extensions::scheduler::argument_providers

constexpr auto extensions::scheduler::argument_providers::EventProviderClosure::operator()(
    app::has_addons_c<addons::EventsTag> auto& app
)
{
    return EventProvider{ app.event_manager };
}
