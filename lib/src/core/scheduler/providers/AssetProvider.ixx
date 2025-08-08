module;

#include <functional>
#include <type_traits>

#include "utility/contracts_macros.hpp"

export module core.scheduler.providers.AssetProvider;

import addons.Assets;

import app;

import core.assets;
import core.scheduler.ProviderFor;

import core.scheduler.accessors.assets;
import core.scheduler.ProviderOf;

import utility.contracts;
import utility.meta.algorithms.for_each;
import utility.meta.concepts.specialization_of;

namespace core::scheduler::providers {

export class AssetProvider {
public:
    template <app::has_addons_c<addons::Assets> App_T>
    explicit AssetProvider(App_T& app);

    template <util::meta::specialization_of_c<accessors::assets::Cached> Cached_T>
    [[nodiscard]]
    auto provide() const -> Cached_T;

private:
    std::reference_wrapper<core::assets::AssetManager> m_asset_manager_ref;
};

}   // namespace core::scheduler::providers

template <>
struct core::scheduler::ProviderOf<addons::Assets>
    : std::type_identity<core::scheduler::providers::AssetProvider> {};

template <typename Loader_T>
struct core::scheduler::
    ProviderFor<core::scheduler::accessors::assets::Cached<Loader_T>>
    : std::type_identity<core::scheduler::providers::AssetProvider> {};

template <app::has_addons_c<addons::Assets> App_T>
core::scheduler::providers::AssetProvider::AssetProvider(App_T& app)
    : m_asset_manager_ref{ app.asset_manager }
{}

template <util::meta::specialization_of_c<core::scheduler::accessors::assets::Cached>
              Cached_T>
auto core::scheduler::providers::AssetProvider::provide() const -> Cached_T
{
    using Loader = typename Cached_T::Loader;

    PRECOND(m_asset_manager_ref.get().contains<Loader>());

    return Cached_T{ m_asset_manager_ref.get().at<Loader>() };
}
