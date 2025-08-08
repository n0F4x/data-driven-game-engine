module;

#include <utility>

export module modules.assets.AssetManager;

import modules.assets.Cached;
import modules.assets.loader_c;
import modules.store.Store;

import utility.containers.StackedTuple;
import utility.meta.concepts.all_different;
import utility.meta.type_traits.forward_like;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.TypeList;

namespace modules::assets {

export class AssetManager {
public:
    explicit AssetManager(modules::store::Store&& cached_loaders);

    template <modules::assets::loader_c Loader_T, typename Self_T>
    [[nodiscard]]
    auto at(this Self_T&&) -> util::meta::forward_like_t<Cached<Loader_T>, Self_T>;

    template <modules::assets::loader_c Loader_T>
    [[nodiscard]]
    auto contains() const noexcept -> bool;

private:
    modules::store::Store m_cached_loaders;
};

}   // namespace modules::assets

modules::assets::AssetManager::AssetManager(modules::store::Store&& cached_loaders)
    : m_cached_loaders{ std::move(cached_loaders) }
{}

template <modules::assets::loader_c Loader_T, typename Self_T>
auto modules::assets::AssetManager::at(this Self_T&& self)
    -> util::meta::forward_like_t<Cached<Loader_T>, Self_T>
{
    return std::forward_like<Self_T>(self.m_cached_loaders).template at<Cached<Loader_T>>();
}

template <modules::assets::loader_c Loader_T>
auto modules::assets::AssetManager::contains() const noexcept -> bool
{
    return m_cached_loaders.contains<Cached<Loader_T>>();
}
