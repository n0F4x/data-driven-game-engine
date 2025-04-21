export module extensions.scheduler.dependency_providers.resource;

import addons.ResourceManager;

import core.app.has_addons_c;

export import extensions.scheduler.dependency_providers.resource.DependencyProvider;
export import extensions.scheduler.dependency_providers.resource.accessors;

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
    return resource::DependencyProvider{ app.resource_manager };
}
