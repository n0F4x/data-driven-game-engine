export module extensions.scheduler.argument_providers.events;

import addons.EventManager;

import core.app.has_addons_c;

export import extensions.scheduler.argument_providers.events.ArgumentProvider;

namespace extensions::scheduler::argument_providers {

export struct EventManager {
    [[nodiscard]]
    constexpr static auto
        operator()(core::app::has_addons_c<addons::EventManagerTag> auto&);
};

}   // namespace extensions::scheduler::argument_providers

constexpr auto extensions::scheduler::argument_providers::EventManager::operator()(
    core::app::has_addons_c<addons::EventManagerTag> auto& app
)
{
    return events::ArgumentProvider{ app.event_manager };
}
