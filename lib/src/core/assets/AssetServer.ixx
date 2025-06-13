module;

#include <cassert>
#include <concepts>
#include <filesystem>
#include <tuple>
#include <type_traits>
#include <unordered_map>

export module core.assets.AssetServer;

import core.assets.loader_c;

import utility.containers.OptionalRef;
import utility.meta.concepts.all_different;
import utility.meta.type_traits.const_like;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.meta.type_traits.type_list.type_list_filter;
import utility.meta.type_traits.type_list.type_list_single;
import utility.TypeList;

template <typename Asset_T>
struct AssetTraits {
    template <typename T>
    struct IsLoaderForAsset : std::is_same<typename T::Asset, Asset_T> {};
};

namespace core::assets {

export template <loader_c... Loaders_T>
    requires util::meta::all_different_c<typename Loaders_T::Asset...>
class AssetServer {
public:
    template <typename Asset_T>
    using loader_for_t = util::meta::type_list_single_t<util::meta::type_list_filter_t<
        util::TypeList<Loaders_T...>,
        ::AssetTraits<Asset_T>::template IsLoaderForAsset>>;

    template <typename Asset_T>
    constexpr static std::bool_constant<
        util::meta::
            type_list_contains_v<util::TypeList<typename Loaders_T::Asset...>, Asset_T>>
        supports_asset_type;

    template <typename... ULoaders_T>
        requires(std::constructible_from<Loaders_T, ULoaders_T &&> && ...)
    explicit AssetServer(ULoaders_T&&... loaders);

    template <typename Asset_T>
    auto load(const std::filesystem::path& path)
        -> Asset_T& requires(supports_asset_type<Asset_T>());

    template <typename Asset_T, typename Self_T>
    [[nodiscard]]
    auto find(this Self_T&, const std::filesystem::path& path)
        -> util::OptionalRef<util::meta::const_like_t<Asset_T, Self_T>>
        requires(supports_asset_type<Asset_T>());

private:
    template <typename Asset_T>
    using AssetMap = std::unordered_map<std::filesystem::path, Asset_T>;

    std::tuple<Loaders_T...>                           m_loaders;
    std::tuple<AssetMap<typename Loaders_T::Asset>...> m_asset_maps;
};

}   // namespace core::assets

template <core::assets::loader_c... Loaders_T>
    requires util::meta::all_different_c<typename Loaders_T::Asset...>
template <typename... ULoaders_T>
    requires(std::constructible_from<Loaders_T, ULoaders_T &&> && ...)
core::assets::AssetServer<Loaders_T...>::AssetServer(ULoaders_T&&... loaders)
    : m_loaders{ std::forward<ULoaders_T>(loaders)... }
{}

template <core::assets::loader_c... Loaders_T>
    requires util::meta::all_different_c<typename Loaders_T::Asset...>
template <typename Asset_T>
auto core::assets::AssetServer<Loaders_T...>::load(const std::filesystem::path& path)
    -> Asset_T& requires(supports_asset_type<Asset_T>())   //
{
    return find<Asset_T>(path).or_else([this, &path] -> Asset_T& {
        const auto [iter, inserted]{
            std::get<AssetMap<Asset_T>>(m_asset_maps)
                .try_emplace(path, std::get<loader_for_t<Asset_T>>(m_loaders).load(path))
        };
        assert(inserted);
        return iter->second;
    });
}

template <core::assets::loader_c... Loaders_T>
    requires util::meta::all_different_c<typename Loaders_T::Asset...>
template <typename Asset_T, typename Self_T>
auto core::assets::AssetServer<Loaders_T...>::find(
    this Self_T&                 self,
    const std::filesystem::path& path
) -> util::OptionalRef<util::meta::const_like_t<Asset_T, Self_T>>
    requires(supports_asset_type<Asset_T>())
{
    auto& asset_map{ std::get<AssetMap<Asset_T>>(self.m_asset_maps) };
    if (const auto iter{ asset_map.find(path) }; iter != asset_map.cend()) {
        return *iter;
    }
    return std::nullopt;
}
