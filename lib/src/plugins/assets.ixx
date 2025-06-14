module;

#include <concepts>
#include <utility>

export module plugins.assets;

import addons.Assets;

import app.Builder;
import app.decays_to_app_c;
import app.decays_to_builder_c;
import core.assets;

import utility.meta.type_traits.type_list.type_list_contains;
import utility.meta.type_traits.forward_like;
import utility.TypeList;

namespace plugins {

export struct AssetsTag {};

export template <core::assets::loader_c... Loaders_T>
class BasicAssets : public AssetsTag {
public:
    BasicAssets() = default;

    template <app::decays_to_builder_c Self_T, core::assets::loader_c Loader_T>
        requires core::assets::loader_c<std::remove_cvref_t<Loader_T>>
              && (!util::meta::type_list_contains_v<util::TypeList<Loaders_T...>, Loader_T>)
    constexpr auto use_loader(this Self_T&&, Loader_T&& loader);

    template <app::decays_to_app_c App_T>
    [[nodiscard]]
    constexpr auto build(App_T&& app);

private:
    std::tuple<Loaders_T...> m_loaders;

    template <core::assets::loader_c...>
    friend class BasicAssets;

    template <typename... ULoaders_T>
        requires(std::constructible_from<Loaders_T, ULoaders_T &&> && ...)
    constexpr explicit BasicAssets(ULoaders_T&&... loaders);
};

export using Assets = BasicAssets<>;

export inline constexpr Assets assets;

}   // namespace plugins

template <core::assets::loader_c... Loaders_T>
template <app::decays_to_builder_c Self_T, core::assets::loader_c Loader_T>
    requires core::assets::loader_c<std::remove_cvref_t<Loader_T>>
          && (!util::meta::type_list_contains_v<util::TypeList<Loaders_T...>, Loader_T>)
constexpr auto plugins::BasicAssets<Loaders_T...>::use_loader(
    this Self_T&& self,
    Loader_T&&    loader
)
{
    return app::swap_plugin<BasicAssets>(
        std::forward<Self_T>(self),
        [&loader](util::meta::forward_like_t<BasicAssets, Self_T> old) {
            return BasicAssets<Loaders_T..., Loader_T>{
                std::get<Loaders_T>(old.m_loaders)..., std::forward<Loader_T>(loader)
            };
        }
    );
}

template <core::assets::loader_c... Loaders_T>
template <app::decays_to_app_c App_T>
constexpr auto plugins::BasicAssets<Loaders_T...>::build(App_T&& app)
{
    return std::forward<App_T>(app).add_on(
        addons::Assets<Loaders_T...>{
            .asset_server = core::assets::AssetServer<Loaders_T...>{ std::get<Loaders_T>(
                m_loaders
            )... },
        }
    );
}

template <core::assets::loader_c... Loaders_T>
template <typename... ULoaders_T>
    requires(std::constructible_from<Loaders_T, ULoaders_T &&> && ...)
constexpr plugins::BasicAssets<Loaders_T...>::BasicAssets(ULoaders_T&&... loaders)
    : m_loaders{ std::forward<ULoaders_T>(loaders)... }
{}
