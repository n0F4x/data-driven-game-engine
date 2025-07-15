module;

#include <functional>
#include <type_traits>
#include <utility>

export module extensions.scheduler.argument_providers.AssetProvider;

import app;

import core.assets;

import extensions.scheduler.accessors.assets;

import utility.containers.StackedTuple;
import utility.meta.concepts.specialization_of;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.meta.type_traits.type_list.type_list_to;
import utility.meta.type_traits.type_list.type_list_transform;
import utility.meta.type_traits.underlying;
import utility.TypeList;

namespace extensions::scheduler::argument_providers {

export template <typename AssetsAddon_T>
class AssetProvider {
    using CachedLoaders = util::meta::type_list_to_t<
        std::remove_reference_t<decltype(std::declval<AssetsAddon_T>().asset_loaders)>,
        util::TypeList>;

    using Loaders =
        util::meta::type_list_transform_t<CachedLoaders, util::meta::underlying>;

public:
    template <app::has_addons_c<AssetsAddon_T> App_T>
    explicit AssetProvider(App_T& app);

    template <typename Accessor_T>
        requires util::meta::
            specialization_of_c<std::remove_cvref_t<Accessor_T>, accessors::assets::Cached>
        [[nodiscard]]
        auto provide() const -> std::remove_cvref_t<Accessor_T>
        requires(util::meta::type_list_contains_v<
                 Loaders,
                 util::meta::underlying_t<std::remove_cvref_t<Accessor_T>>>);

private:
    std::reference_wrapper<util::meta::type_list_to_t<CachedLoaders, util::StackedTuple>>
        m_asset_loaders;
};

}   // namespace extensions::scheduler::argument_providers

template <typename AssetsAddon_T>
template <app::has_addons_c<AssetsAddon_T> App_T>
extensions::scheduler::argument_providers::AssetProvider<AssetsAddon_T>::AssetProvider(
    App_T& app
)
    : m_asset_loaders{ app.asset_loaders }
{}

template <typename AssetsAddon_T>
template <typename Accessor_T>
    requires util::meta::specialization_of_c<
        std::remove_cvref_t<Accessor_T>,
        extensions::scheduler::accessors::assets::Cached>
auto extensions::scheduler::argument_providers::AssetProvider<AssetsAddon_T>::provide(
) const -> std::remove_cvref_t<Accessor_T>
    requires(util::meta::type_list_contains_v<
             Loaders,
             util::meta::underlying_t<std::remove_cvref_t<Accessor_T>>>)
{
    return std::remove_cvref_t<Accessor_T>{
        m_asset_loaders.get()
            .template get<core::assets::Cached<
                util::meta::underlying_t<std::remove_cvref_t<Accessor_T>>>>()
    };
}
