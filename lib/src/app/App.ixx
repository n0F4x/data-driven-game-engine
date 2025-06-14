module;

#include <concepts>
#include <type_traits>
#include <utility>

export module app.App;

import utility.meta.algorithms.apply;
import utility.meta.concepts.specialization_of;
import utility.meta.type_traits.forward_like;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.meta.type_traits.type_list.type_list_replace;
import utility.TypeList;

import app.addon_c;
import app.decays_to_addon_c;

template <typename Transform_T, typename App_T, typename OldAddon_T>
concept valid_swap_addon_transform_c =
    requires { util::meta::type_list_contains_v<std::remove_cvref_t<App_T>, OldAddon_T>; }
    && app::addon_c<
        std::invoke_result_t<Transform_T&&, util::meta::forward_like_t<OldAddon_T, App_T>>>
    && (!util::meta::type_list_contains_v<
        std::remove_cvref_t<App_T>,
        std::invoke_result_t<Transform_T&&, util::meta::forward_like_t<OldAddon_T, App_T>>>);

namespace app {

export template <addon_c... Addons_T>
class App;

export template <
    typename OldAddon_T,
    typename App_T,
    valid_swap_addon_transform_c<App_T, OldAddon_T> Transform_T>
    requires util::meta::specialization_of_c<std::remove_cvref_t<App_T>, App>
[[nodiscard]]
constexpr auto swap_addon(App_T&& app, Transform_T&& transform_addon)
    -> util::meta::type_list_replace_t<
        std::remove_cvref_t<App_T>,
        OldAddon_T,
        std::invoke_result_t<Transform_T&&, util::meta::forward_like_t<OldAddon_T, App_T>>>;

export template <addon_c... Addons_T>
class App : public Addons_T... {
public:
    App()
        requires(sizeof...(Addons_T) == 0)
    = default;

    template <typename Self_T, decays_to_addon_c Addon_T>
        requires(!util::meta::type_list_contains_v<util::TypeList<Addons_T...>, Addon_T>)
    [[nodiscard]]
    constexpr auto add_on(this Self_T&&, Addon_T&& addon)
        -> App<Addons_T..., std::remove_cvref_t<Addon_T>>;

private:
    template <
        typename OldAddon_T,
        typename App_T,
        valid_swap_addon_transform_c<App_T, OldAddon_T> Transform_T>
        requires util::meta::specialization_of_c<std::remove_cvref_t<App_T>, ::app::App>
    [[nodiscard]]
    constexpr auto swap_addon(App_T&& app, Transform_T&& transform_addon)
        -> util::meta::type_list_replace_t<
            std::remove_cvref_t<App_T>,
            OldAddon_T,
            std::invoke_result_t<Transform_T&&, util::meta::forward_like_t<OldAddon_T, App_T>>>;

    template <addon_c...>
    friend class App;

    template <typename... UAddons_T>
        requires(std::constructible_from<Addons_T, UAddons_T &&> && ...)
    constexpr explicit App(UAddons_T&&... addons);
};

}   // namespace app

template <
    typename OldAddon_T,
    typename App_T,
    valid_swap_addon_transform_c<App_T, OldAddon_T> Transform_T>
    requires util::meta::specialization_of_c<std::remove_cvref_t<App_T>, app::App>
constexpr auto app::swap_addon(App_T&& app, Transform_T&& transform_addon)
    -> util::meta::type_list_replace_t<
        std::remove_cvref_t<App_T>,
        OldAddon_T,
        std::invoke_result_t<Transform_T&&, util::meta::forward_like_t<OldAddon_T, App_T>>>
{
    using Result = util::meta::type_list_replace_t<
        std::remove_cvref_t<App_T>,
        OldAddon_T,
        std::invoke_result_t<Transform_T&&, util::meta::forward_like_t<OldAddon_T, App_T>>>;

    return util::meta::apply<std::remove_cvref_t<App_T>>(
        [&app, &transform_addon]<typename... Addons_T> -> Result {
            return Result{ [&app, &transform_addon] {
                if constexpr (std::is_same_v<Addons_T, OldAddon_T>) {
                    return std::invoke(
                        std::forward<Transform_T>(transform_addon),
                        static_cast<util::meta::forward_like_t<Addons_T, App_T>>(app)
                    );
                }
                else {
                    std::ignore = transform_addon;
                    return static_cast<util::meta::forward_like_t<Addons_T, App_T>>(app);
                }
            }()... };
        }
    );
}

template <app::addon_c... Addons_T>
template <typename Self_T, app::decays_to_addon_c Addon_T>
    requires(!util::meta::type_list_contains_v<util::TypeList<Addons_T...>, Addon_T>)
constexpr auto app::App<Addons_T...>::add_on(this Self_T&& self, Addon_T&& addon)
    -> App<Addons_T..., std::remove_cvref_t<Addon_T>>
{
    return App<Addons_T..., std::remove_cvref_t<Addon_T>>{
        static_cast<util::meta::forward_like_t<Addons_T, Self_T>>(self)...,
        std::forward<Addon_T>(addon)
    };
}

template <app::addon_c... Addons_T>
template <typename... UAddons_T>
    requires(std::constructible_from<Addons_T, UAddons_T &&> && ...)
constexpr app::App<Addons_T...>::App(UAddons_T&&... addons)
    : Addons_T{ std::forward<UAddons_T>(addons) }...
{}
