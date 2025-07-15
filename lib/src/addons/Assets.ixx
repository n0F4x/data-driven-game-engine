export module addons.Assets;

import core.assets;

import extensions.scheduler.argument_provider_for;
import extensions.scheduler.argument_providers.AssetProvider;

import utility.containers.StackedTuple;

namespace addons {

export template <core::assets::loader_c... Loaders_T>
struct Assets {
    util::StackedTuple<core::assets::Cached<Loaders_T>...> asset_loaders;
};

}   // namespace addons

template <typename... Assets_T>
struct extensions::scheduler::ArgumentProviderFor<addons::Assets<Assets_T...>> {
    using type = extensions::scheduler::argument_providers::
        AssetProvider<addons::Assets<Assets_T...>>;
};
