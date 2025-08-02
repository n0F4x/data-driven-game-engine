export module addons.Resources;

import core.resources;

import extensions.scheduler.provider_for;
import extensions.scheduler.providers.ResourceProvider;

namespace addons {

export struct ResourcesTag {};

export template <core::resources::resource_c... Resources_T>
struct Resources : ResourcesTag {
    core::resources::ResourceManager<Resources_T...> resource_manager;
};

}   // namespace addons

template <core::resources::resource_c... Resources_T>
struct extensions::scheduler::ProviderFor<addons::Resources<Resources_T...>> {
    using type = extensions::scheduler::providers::ResourceProvider<
        core::resources::ResourceManager<Resources_T...>,
        addons::Resources<Resources_T...>>;
};
