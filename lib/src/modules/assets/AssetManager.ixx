module;

#include <utility>

export module ddge.modules.assets.AssetManager;

import ddge.modules.assets.Cached;
import ddge.modules.assets.loader_c;
import ddge.utility.containers.store.Store;

import ddge.utility.meta.concepts.all_different;
import ddge.utility.meta.type_traits.forward_like;
import ddge.utility.meta.type_traits.type_list.type_list_contains;
import ddge.utility.TypeList;

namespace ddge::assets {

export class AssetManager {
public:
    explicit AssetManager(ddge::util::store::Store&& cached_loaders);

    template <ddge::assets::loader_c Loader_T, typename Self_T>
    [[nodiscard]]
    auto at(this Self_T&&) -> util::meta::forward_like_t<Cached<Loader_T>, Self_T>;

    template <ddge::assets::loader_c Loader_T>
    [[nodiscard]]
    auto contains() const noexcept -> bool;

private:
    ddge::util::store::Store m_cached_loaders;
};

}   // namespace ddge::assets

ddge::assets::AssetManager::AssetManager(ddge::util::store::Store&& cached_loaders)
    : m_cached_loaders{ std::move(cached_loaders) }
{}

template <ddge::assets::loader_c Loader_T, typename Self_T>
auto ddge::assets::AssetManager::at(this Self_T&& self)
    -> util::meta::forward_like_t<Cached<Loader_T>, Self_T>
{
    return std::forward_like<Self_T>(self.m_cached_loaders).template at<Cached<Loader_T>>();
}

template <ddge::assets::loader_c Loader_T>
auto ddge::assets::AssetManager::contains() const noexcept -> bool
{
    return m_cached_loaders.contains<Cached<Loader_T>>();
}
