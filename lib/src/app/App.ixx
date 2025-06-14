module;

#include <concepts>
#include <type_traits>

export module app.App;

import utility.meta.type_traits.forward_like;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.TypeList;

import app.addon_c;
import app.decays_to_addon_c;

namespace app {

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
    template <addon_c...>
    friend class App;

    template <typename... UAddons_T>
        requires(std::constructible_from<Addons_T, UAddons_T&&> && ...)
    constexpr explicit App(UAddons_T&&... addons);
};

}   // namespace app

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
    requires(std::constructible_from<Addons_T, UAddons_T&&> && ...)
constexpr app::App<Addons_T...>::App(UAddons_T&&... addons)
    : Addons_T{ std::forward<UAddons_T>(addons) }...
{}
