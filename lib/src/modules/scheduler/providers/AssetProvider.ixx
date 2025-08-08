module;

#include <functional>
#include <type_traits>

#include "utility/contracts_macros.hpp"

export module modules.scheduler.providers.AssetProvider;

import addons.Assets;

import app;

import modules.assets;
import modules.scheduler.ProviderFor;

import modules.scheduler.accessors.assets;
import modules.scheduler.ProviderOf;

import utility.contracts;
import utility.meta.algorithms.for_each;
import utility.meta.concepts.specialization_of;

namespace modules::scheduler::providers {

export class AssetProvider {
public:
    template <app::has_addons_c<addons::Assets> App_T>
    explicit AssetProvider(App_T& app);

    template <util::meta::specialization_of_c<accessors::assets::Cached> Cached_T>
    [[nodiscard]]
    auto provide() const -> Cached_T;

private:
    std::reference_wrapper<modules::assets::AssetManager> m_asset_manager_ref;
};

}   // namespace modules::scheduler::providers

template <>
struct modules::scheduler::ProviderOf<addons::Assets>
    : std::type_identity<modules::scheduler::providers::AssetProvider> {};

template <typename Loader_T>
struct modules::scheduler::
    ProviderFor<modules::scheduler::accessors::assets::Cached<Loader_T>>
    : std::type_identity<modules::scheduler::providers::AssetProvider> {};

template <app::has_addons_c<addons::Assets> App_T>
modules::scheduler::providers::AssetProvider::AssetProvider(App_T& app)
    : m_asset_manager_ref{ app.asset_manager }
{}

template <util::meta::specialization_of_c<modules::scheduler::accessors::assets::Cached>
              Cached_T>
auto modules::scheduler::providers::AssetProvider::provide() const -> Cached_T
{
    using Loader = typename Cached_T::Loader;

    PRECOND(m_asset_manager_ref.get().contains<Loader>());

    return Cached_T{ m_asset_manager_ref.get().at<Loader>() };
}
