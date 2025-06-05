export module extensions.scheduler.argument_providers.events;

import addons.EventManager;

import core.app.has_addons_c;

export import extensions.scheduler.argument_providers.events.EventProvider;

namespace extensions::scheduler::argument_providers {

export struct EventProviderClosure {
    [[nodiscard]]
    constexpr static auto
        operator()(core::app::has_addons_c<addons::EventManagerTag> auto&);
};

export constexpr EventProviderClosure event_provider;

}   // namespace extensions::scheduler::argument_providers

constexpr auto extensions::scheduler::argument_providers::EventProviderClosure::operator()(
    core::app::has_addons_c<addons::EventManagerTag> auto& app
)
{
    return EventProvider{ app.event_manager };
}
