module;

#include <functional>
#include <string>
#include <type_traits>
#include <utility>

export module plugins.assets;

import addons.Assets;

import app;

import core.assets;

import utility.containers.StackedTuple;
import utility.meta.algorithms.apply;
import utility.meta.algorithms.for_each;
import utility.meta.reflection.name_of;
import utility.meta.type_traits.back;
import utility.meta.type_traits.functional.arguments_of;
import utility.meta.type_traits.functional.result_of;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.meta.type_traits.type_list.type_list_drop_back;
import utility.tuple;
import utility.TypeList;

namespace plugins {

export template <typename T>
concept asset_loader_injection_c = core::assets::loader_c<util::meta::result_of_t<T>>;

export template <typename T>
concept decays_to_asset_loader_injection_c = asset_loader_injection_c<std::decay_t<T>>;

export struct AssetsTag {};

template <plugins::asset_loader_injection_c... Injections_T>
class BasicAssets;

template <typename... Injections_T>
using old_assets_t =
    util::meta::type_list_drop_back_t<plugins::BasicAssets<Injections_T...>>;

template <plugins::asset_loader_injection_c... Injections_T>
class BasicAssets : public AssetsTag {
public:
    BasicAssets() = default;

    template <typename OldBasicAssets_T, typename... Args_T>
        requires std::
            same_as<std::remove_cvref_t<OldBasicAssets_T>, old_assets_t<Injections_T...>>
        constexpr BasicAssets(
            OldBasicAssets_T&& old_assets,
            std::in_place_t,
            Args_T&&... args
        );

    template <app::decays_to_builder_c Self_T, typename Loader_T>
        requires core::assets::loader_c<std::remove_cvref_t<Loader_T>>
    constexpr auto insert_loader(this Self_T&&, Loader_T&& loader);

    template <
        app::decays_to_builder_c                    Self_T,
        plugins::decays_to_asset_loader_injection_c Injection_T>
    constexpr auto inject_loader(this Self_T&&, Injection_T&& injection);

    template <app::decays_to_app_c App_T>
    [[nodiscard]]
    constexpr auto build(App_T&& app) &&;

private:
    template <plugins::asset_loader_injection_c...>
    friend class BasicAssets;

    std::tuple<std::decay_t<Injections_T>...> m_injections;
};

export using Assets = BasicAssets<>;

export inline constexpr Assets assets;

}   // namespace plugins

template <plugins::asset_loader_injection_c... Injections_T>
template <typename OldBasicAssets_T, typename... Args>
    requires std::same_as<
        std::remove_cvref_t<OldBasicAssets_T>,
        plugins::old_assets_t<Injections_T...>>
constexpr plugins::BasicAssets<Injections_T...>::BasicAssets(
    OldBasicAssets_T&& old_assets,
    std::in_place_t,
    Args&&... args
)
    : m_injections{ std::tuple_cat(
          std::forward_like<OldBasicAssets_T>(old_assets.m_injections),
          std::make_tuple(
              std::decay_t<util::meta::back_t<Injections_T...>>(std::forward<Args>(args
              )...)
          )
      ) }
{}

template <plugins::asset_loader_injection_c... Injections_T>
template <app::decays_to_builder_c Self_T, typename Loader_T>
    requires core::assets::loader_c<std::remove_cvref_t<Loader_T>>
constexpr auto plugins::BasicAssets<Injections_T...>::insert_loader(
    this Self_T&& self,
    Loader_T&&    loader
)
{
    using Loader = std::remove_cvref_t<Loader_T>;

    struct Injection {
        constexpr auto operator()() && -> Loader
        {
            return std::move(loader);
        }

        Loader loader;
    };

    return std::forward<Self_T>(self).inject_loader(
        Injection{ std::forward<Loader_T>(loader) }
    );
}

template <typename>
struct gather_helper;

template <template <typename...> typename TypeList_T, typename... SelectedTypes_T>
struct gather_helper<TypeList_T<SelectedTypes_T...>> {
    template <typename... Ts>
    [[nodiscard]]
    constexpr static auto operator()(std::tuple<Ts...>& tuple)
        -> std::tuple<SelectedTypes_T...>
    {
        return { std::forward_like<SelectedTypes_T>(
            std::get<std::remove_cvref_t<SelectedTypes_T>>(tuple)
        )... };
    }
};

template <typename Callable_T, typename... Ts>
constexpr auto gather_parameters(std::tuple<Ts...>& tuple)
{
    using namespace std::literals::string_literals;

    using RequiredCachedLoadersTuple_T = util::meta::arguments_of_t<Callable_T>;

    util::meta::for_each<RequiredCachedLoadersTuple_T>([]<typename Dependency_T> {
        static_assert(
            util::meta::type_list_contains_v<
                util::TypeList<Ts...>,
                std::remove_cvref_t<Dependency_T>>,
            // TODO: constexpr std::format
            "Dependency `"s + util::meta::name_of<std::remove_cvref_t<Dependency_T>>()
                + "` not found for `" + util::meta::name_of<Callable_T>() + "`"
        );
    });

    return gather_helper<RequiredCachedLoadersTuple_T>::operator()(tuple);
}

template <plugins::asset_loader_injection_c... Injections_T>
template <
    app::decays_to_builder_c                    Self_T,
    plugins::decays_to_asset_loader_injection_c Injection_T>
constexpr auto plugins::BasicAssets<Injections_T...>::inject_loader(
    this Self_T&& self,
    Injection_T&& injection
)
{
    return app::swap_plugin<BasicAssets>(
        std::forward<Self_T>(self),
        [&]<typename BasicAssets_T>
            requires(std::is_same_v<std::remove_cvref_t<BasicAssets_T>, BasicAssets>)
        (BasicAssets_T&& assets) {
            return BasicAssets<Injections_T..., std::decay_t<Injection_T>>{
                std::forward<BasicAssets_T>(assets),
                std::in_place,
                std::forward<Injection_T>(injection)
            };
        }
        );
}

template <plugins::asset_loader_injection_c... Injections_T>
template <app::decays_to_app_c App_T>
constexpr auto plugins::BasicAssets<Injections_T...>::build(App_T&& app) &&
{
    using AssetsAddon = addons::Assets<util::meta::result_of_t<Injections_T>...>;

    static_assert(!app::has_addons_c<App_T, AssetsAddon>);

    return util::meta::apply<std::make_index_sequence<sizeof...(Injections_T)>>(
        [this, &app]<std::size_t... Is> {
            std::ignore = this;
            return std::forward<App_T>(app).add_on(
                addons::Assets<util::meta::result_of_t<Injections_T>...>{
                    .asset_manager{ std::get<Injections_T>(std::move(m_injections))... },
                }
            );
        }
    );
}
