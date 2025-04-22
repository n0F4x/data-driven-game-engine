export module extensions.scheduler.dependency_providers.events;

import addons.EventManager;

import core.app.has_addons_c;

export import extensions.scheduler.dependency_providers.events.DependencyProvider;

namespace extensions::scheduler::dependency_providers {

export struct EventManager {
    [[nodiscard]]
    constexpr static auto
        operator()(core::app::has_addons_c<addons::EventManagerTag> auto&);
};

}   // namespace extensions::scheduler::dependency_providers

constexpr auto extensions::scheduler::dependency_providers::EventManager::operator()(
    core::app::has_addons_c<addons::EventManagerTag> auto& app
)
{
    return events::DependencyProvider{ app.event_manager };
}
