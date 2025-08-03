module;

#include <functional>
#include <type_traits>

#include "utility/contracts_macros.hpp"

export module extensions.scheduler.providers.AssetProvider;

import addons.Assets;

import app;

import core.assets;
import core.scheduler.ProviderFor;
import core.store;

import extensions.scheduler.accessors.assets;
import extensions.scheduler.ProviderOf;

import utility.contracts;
import utility.meta.algorithms.for_each;
import utility.meta.concepts.specialization_of;

namespace extensions::scheduler::providers {

export class AssetProvider {
public:
    template <app::has_addons_c<addons::AssetsTag> App_T>
    explicit AssetProvider(App_T& app);

    template <util::meta::specialization_of_c<accessors::assets::Cached> Cached_T>
    [[nodiscard]]
    auto provide() const -> Cached_T;

private:
    core::store::Store m_cached_loader_refs;
};

}   // namespace extensions::scheduler::providers

template <typename... Loaders_T>
struct extensions::scheduler::ProviderOf<addons::Assets<Loaders_T...>>
    : std::type_identity<extensions::scheduler::providers::AssetProvider> {};

template <typename Loader_T>
struct core::scheduler::
    ProviderFor<extensions::scheduler::accessors::assets::Cached<Loader_T>>
    : std::type_identity<extensions::scheduler::providers::AssetProvider> {};

template <app::has_addons_c<addons::AssetsTag> App_T>
extensions::scheduler::providers::AssetProvider::AssetProvider(App_T& app)
{
    using AssetManager = std::remove_cvref_t<decltype(app.asset_manager)>;

    util::meta::for_each<typename AssetManager::Loaders>(
        [this, asset_manager = std::ref(app.asset_manager)]<typename Loader_T> {
            m_cached_loader_refs
                .emplace<std::reference_wrapper<core::assets::Cached<Loader_T>>>(
                    asset_manager.get().template get<Loader_T>()
                );
        }
    );
}

template <util::meta::specialization_of_c<extensions::scheduler::accessors::assets::Cached>
              Cached_T>
auto extensions::scheduler::providers::AssetProvider::provide() const -> Cached_T
{
    using Loader          = typename Cached_T::Loader;
    using CachedLoader    = core::assets::Cached<Loader>;
    using CachedLoaderRef = std::reference_wrapper<CachedLoader>;

    PRECOND(m_cached_loader_refs.contains<CachedLoaderRef>());

    return Cached_T{ m_cached_loader_refs.at<CachedLoaderRef>() };
}
