export module extensions.scheduler.argument_providers.resources;

import addons.Resources;

import core.app.has_addons_c;

export import extensions.scheduler.argument_providers.resources.ResourceProvider;

namespace extensions::scheduler::argument_providers {

export struct ResourceProviderClosure {
    [[nodiscard]]
    constexpr static auto operator()(
        core::app::has_addons_c<addons::ResourcesTag> auto& app
    );
};

export constexpr ResourceProviderClosure resource_provider;

}   // namespace extensions::scheduler::argument_providers

constexpr auto extensions::scheduler::argument_providers::ResourceProviderClosure::
    operator()(core::app::has_addons_c<addons::ResourcesTag> auto& app)
{
    return resources::ResourceProvider{ app.resource_manager };
}
