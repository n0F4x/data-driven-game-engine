module;

#include <utility>

export module core.assets.AssetManager;

import core.assets.Cached;
import core.assets.loader_c;
import core.store.Store;

import utility.containers.StackedTuple;
import utility.meta.concepts.all_different;
import utility.meta.type_traits.forward_like;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.TypeList;

namespace core::assets {

export class AssetManager {
public:
    explicit AssetManager(core::store::Store&& cached_loaders);

    template <core::assets::loader_c Loader_T, typename Self_T>
    [[nodiscard]]
    auto at(this Self_T&&) -> util::meta::forward_like_t<Cached<Loader_T>, Self_T>;

    template <core::assets::loader_c Loader_T>
    [[nodiscard]]
    auto contains() const noexcept -> bool;

private:
    core::store::Store m_cached_loaders;
};

}   // namespace core::assets

core::assets::AssetManager::AssetManager(core::store::Store&& cached_loaders)
    : m_cached_loaders{ std::move(cached_loaders) }
{}

template <core::assets::loader_c Loader_T, typename Self_T>
auto core::assets::AssetManager::at(this Self_T&& self)
    -> util::meta::forward_like_t<Cached<Loader_T>, Self_T>
{
    return std::forward_like<Self_T>(self.m_cached_loaders).template at<Cached<Loader_T>>();
}

template <core::assets::loader_c Loader_T>
auto core::assets::AssetManager::contains() const noexcept -> bool
{
    return m_cached_loaders.contains<Cached<Loader_T>>();
}
