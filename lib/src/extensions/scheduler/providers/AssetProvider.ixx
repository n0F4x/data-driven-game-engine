module;

#include <functional>
#include <type_traits>
#include <utility>

export module extensions.scheduler.providers.AssetProvider;

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

namespace extensions::scheduler::providers {

export template <typename AssetManager_T, typename AssetsAddon_T>
class AssetProvider {
public:
    template <app::has_addons_c<AssetsAddon_T> App_T>
    explicit AssetProvider(App_T& app);

    template <typename Accessor_T>
        requires util::meta::
            specialization_of_c<std::remove_cvref_t<Accessor_T>, accessors::assets::Cached>
        [[nodiscard]]
        auto provide() const -> std::remove_cvref_t<Accessor_T>
        requires(AssetManager_T::template contains<
                 util::meta::underlying_t<std::remove_cvref_t<Accessor_T>>>());

private:
    std::reference_wrapper<AssetManager_T> m_asset_manager;
};

}   // namespace extensions::scheduler::providers

template <typename AssetManager_T, typename AssetsAddon_T>
template <app::has_addons_c<AssetsAddon_T> App_T>
extensions::scheduler::providers::AssetProvider<AssetManager_T, AssetsAddon_T>::
    AssetProvider(App_T& app)
    : m_asset_manager{ app.asset_manager }
{}

template <typename AssetManager_T, typename AssetsAddon_T>
template <typename Accessor_T>
    requires util::meta::specialization_of_c<
        std::remove_cvref_t<Accessor_T>,
        extensions::scheduler::accessors::assets::Cached>
auto extensions::scheduler::providers::
    AssetProvider<AssetManager_T, AssetsAddon_T>::provide() const
    -> std::remove_cvref_t<Accessor_T>
    requires(AssetManager_T::template contains<
             util::meta::underlying_t<std::remove_cvref_t<Accessor_T>>>())
{
    return std::remove_cvref_t<Accessor_T>{
        m_asset_manager.get()
            .template get<util::meta::underlying_t<std::remove_cvref_t<Accessor_T>>>()
    };
}
