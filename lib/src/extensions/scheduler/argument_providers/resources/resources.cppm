export module extensions.scheduler.argument_providers.resources;

import addons.ResourceManager;

import core.app.has_addons_c;

export import extensions.scheduler.argument_providers.resources.ArgumentProvider;

namespace extensions::scheduler::argument_providers {

export struct ResourceManager {
    [[nodiscard]]
    constexpr static auto operator()(
        core::app::has_addons_c<addons::ResourceManagerTag> auto& app
    );
};

}   // namespace extensions::scheduler::argument_providers

constexpr auto extensions::scheduler::argument_providers::ResourceManager::operator()(
    core::app::has_addons_c<addons::ResourceManagerTag> auto& app
)
{
    return resources::ArgumentProvider{ app.resource_manager };
}
