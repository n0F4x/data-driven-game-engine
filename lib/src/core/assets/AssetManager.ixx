module;

#include <concepts>
#include <memory>
#include <type_traits>
#include <utility>

export module core.assets.AssetManager;

import core.assets.Cached;
import core.assets.loader_c;

import utility.containers.StackedTuple;
import utility.meta.concepts.all_different;
import utility.meta.type_traits.forward_like;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.TypeList;

namespace core::assets {

export template <core::assets::loader_c... Loaders_T>
    requires util::meta::all_different_c<Loaders_T...>
class AssetManager {
public:
    template <core::assets::loader_c Loader_T>
    constexpr static std::bool_constant<
        util::meta::type_list_contains_v<util::TypeList<Loaders_T...>, Loader_T>>
        contains;

    template <typename... Factories_T>
        requires std::constructible_from<util::StackedTuple<Loaders_T...>, Factories_T&&...>
    constexpr explicit AssetManager(Factories_T&&... factories);

    template <core::assets::loader_c Loader_T, typename Self_T>
    [[nodiscard]]
    constexpr auto get(this Self_T&&)
        -> util::meta::forward_like_t<Cached<Loader_T>, Self_T>
        requires(contains<Loader_T>());

private:
    std::unique_ptr<util::StackedTuple<Cached<Loaders_T>...>> m_cached_loaders;
};

}   // namespace core::assets

template <core::assets::loader_c... Loaders_T>
    requires util::meta::all_different_c<Loaders_T...>
template <typename... Factories_T>
    requires std::constructible_from<util::StackedTuple<Loaders_T...>, Factories_T&&...>
constexpr core::assets::AssetManager<Loaders_T...>::AssetManager(
    Factories_T&&... factories
)
    : m_cached_loaders{ std::make_unique<util::StackedTuple<Cached<Loaders_T>...>>(
          std::forward<Factories_T>(factories)...
      ) }
{}

template <core::assets::loader_c... Loaders_T>
    requires util::meta::all_different_c<Loaders_T...>
template <core::assets::loader_c Loader_T, typename Self_T>
constexpr auto core::assets::AssetManager<Loaders_T...>::get(this Self_T&& self)
    -> util::meta::forward_like_t<Cached<Loader_T>, Self_T>
    requires(contains<Loader_T>())
{
    return std::forward_like<Self_T>(
        self.m_cached_loaders->template get<Cached<Loader_T>>()
    );
}
