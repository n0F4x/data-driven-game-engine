module;

#include <concepts>
#include <functional>
#include <optional>
#include <type_traits>
#include <unordered_map>

export module core.assets.Cached;

import core.assets.Handle;
import core.assets.loader_c;
import core.assets.WeakHandle;

import utility.containers.FunctionWrapper;
import utility.hashing;
import utility.meta.type_traits.const_like;
import utility.meta.type_traits.functional.arguments_of;
import utility.meta.type_traits.functional.result_of;

namespace core::assets {

template <typename Loader_T, typename Asset_T, typename... Arguments_T>
class CachedImpl {
public:
    using Asset = Asset_T;

    CachedImpl()
        requires std::default_initializable<Loader_T>
    = default;

    template <typename ULoader_T>
        requires(std::constructible_from<Loader_T, ULoader_T &&>)
    explicit CachedImpl(ULoader_T&& loader);

    [[nodiscard("assets are not owned by Cached")]]
    auto load(Arguments_T... arguments) -> Handle<Asset_T>;

    template <typename Self_T>
    [[nodiscard]]
    auto find(this Self_T&&, Arguments_T... arguments) -> std::optional<
        Handle<util::meta::const_like_t<Asset_T, std::remove_reference_t<Self_T>>>>;

private:
    util::FunctionWrapper<Loader_T>                                   m_loader;
    std::unordered_map<std::size_t, WeakHandle<Asset>, std::identity> m_asset_map;

    [[nodiscard]]
    constexpr auto hash(Arguments_T... arguments) const -> std::size_t;

    template <typename Self_T>
    [[nodiscard]]
    auto find_hash(this Self_T&&, std::size_t hash) -> std::optional<
        Handle<util::meta::const_like_t<Asset_T, std::remove_reference_t<Self_T>>>>;
};

template <typename Loader_T, typename Asset_T, typename ArgumentList_T>
struct CachedImplType;

template <
    typename Loader_T,
    typename Asset_T,
    template <typename...> typename TypeList_T,
    typename... Args_T>
struct CachedImplType<Loader_T, Asset_T, TypeList_T<Args_T...>> {
    using type = CachedImpl<Loader_T, Asset_T, Args_T...>;
};

template <typename Loader_T>
using cached_impl_t = typename CachedImplType<
    Loader_T,
    util::meta::result_of_t<Loader_T>,
    util::meta::arguments_of_t<Loader_T>>::type;

export template <loader_c Loader_T>
class Cached : public cached_impl_t<Loader_T> {
    using Base = cached_impl_t<Loader_T>;

public:
    using Base::Base;
};

}   // namespace core::assets

template <typename Loader_T, typename Asset_T, typename... Arguments_T>
template <typename ULoader_T>
    requires(std::constructible_from<Loader_T, ULoader_T &&>)
core::assets::CachedImpl<Loader_T, Asset_T, Arguments_T...>::CachedImpl(ULoader_T&& loader)
    : m_loader{ std::forward<ULoader_T>(loader) }
{}

template <typename Loader_T, typename Asset_T, typename... Arguments_T>
auto core::assets::CachedImpl<Loader_T, Asset_T, Arguments_T...>::load(
    Arguments_T... arguments
) -> Handle<Asset_T>
{
    std::size_t key{ hash(arguments...) };

    if (const auto found_handle{ find_hash(key) }; found_handle.has_value()) {
        return *found_handle;
    }

    Handle handle{ std::make_shared<Asset_T>(
        std::invoke(m_loader, std::forward<Arguments_T>(arguments)...)
    ) };

    m_asset_map.try_emplace(key, handle);

    return handle;
}

template <typename Loader_T, typename Asset_T, typename... Arguments_T>
template <typename Self_T>
auto core::assets::CachedImpl<Loader_T, Asset_T, Arguments_T...>::find(
    this Self_T&& self,
    Arguments_T... arguments
) -> std::
    optional<Handle<util::meta::const_like_t<Asset_T, std::remove_reference_t<Self_T>>>>
{
    return std::forward<Self_T>(self).find_hash(self.hash(arguments...));
}

template <typename Loader_T, typename Asset_T, typename... Arguments_T>
template <typename Self_T>
auto core::assets::CachedImpl<Loader_T, Asset_T, Arguments_T...>::find_hash(
    this Self_T&&     self,
    const std::size_t hash
) -> std::
    optional<Handle<util::meta::const_like_t<Asset_T, std::remove_reference_t<Self_T>>>>
{
    const auto iter{ self.m_asset_map.find(hash) };
    if (iter == self.m_asset_map.cend()) {
        return std::nullopt;
    }

    Handle result{ iter->second.lock() };
    if (result == nullptr) {
        return std::nullopt;
    }

    return result;
}

template <typename Loader_T, typename Asset_T, typename... Arguments_T>
constexpr auto core::assets::CachedImpl<Loader_T, Asset_T, Arguments_T...>::hash(
    Arguments_T... arguments
) const -> std::size_t
{
    return util::hash_combine(arguments...);
}
