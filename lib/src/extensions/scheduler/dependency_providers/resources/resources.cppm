export module extensions.scheduler.dependency_providers.resources;

import addons.ResourceManager;

import core.app.has_addons_c;

export import extensions.scheduler.dependency_providers.resources.DependencyProvider;

namespace extensions::scheduler::dependency_providers {

export struct ResourceManager {
    [[nodiscard]]
    constexpr static auto operator()(
        core::app::has_addons_c<addons::ResourceManagerTag> auto& app
    );
};

}   // namespace extensions::scheduler::dependency_providers

constexpr auto extensions::scheduler::dependency_providers::ResourceManager::operator()(
    core::app::has_addons_c<addons::ResourceManagerTag> auto& app
)
{
    return resources::DependencyProvider{ app.resource_manager };
}
