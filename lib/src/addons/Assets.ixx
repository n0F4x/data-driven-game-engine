export module addons.Assets;

import core.assets;

import extensions.scheduler.argument_provider_for;
import extensions.scheduler.argument_providers.AssetProvider;

import utility.containers.StackedTuple;

namespace addons {

export template <core::assets::loader_c... Loaders_T>
struct Assets {
    core::assets::AssetManager<Loaders_T...> asset_manager;
};

}   // namespace addons

template <typename... Loaders_T>
struct extensions::scheduler::ArgumentProviderFor<addons::Assets<Loaders_T...>> {
    using type = extensions::scheduler::argument_providers::AssetProvider<
        core::assets::AssetManager<Loaders_T...>,
        addons::Assets<Loaders_T...>>;
};
