module;

#include <concepts>
#include <type_traits>
#include <utility>

export module ddge.modules.app.App;

import ddge.utility.meta.type_traits.forward_like;
import ddge.utility.meta.type_traits.type_list.type_list_contains;
import ddge.utility.TypeList;

import ddge.modules.app.addon_c;
import ddge.modules.app.decays_to_addon_c;

namespace ddge::app {

export template <addon_c... Addons_T>
class App : public Addons_T... {
public:
    App() = default;

    using Addons = util::TypeList<Addons_T...>;

    template <typename Self_T, decays_to_addon_c Addon_T>
        requires(!util::meta::type_list_contains_v<util::TypeList<Addons_T...>, Addon_T>)
    [[nodiscard]]
    constexpr auto add_on(this Self_T&&, Addon_T&& addon)
        -> App<Addons_T..., std::remove_cvref_t<Addon_T>>;

private:
    template <addon_c...>
    friend class App;

    template <typename... UAddons_T>
        requires(std::constructible_from<Addons_T, UAddons_T &&> && ...)
    constexpr explicit App(UAddons_T&&... addons);
};

}   // namespace ddge::app

template <ddge::app::addon_c... Addons_T>
template <typename Self_T, ddge::app::decays_to_addon_c Addon_T>
    requires(
        !ddge::util::meta::type_list_contains_v<ddge::util::TypeList<Addons_T...>, Addon_T>
    )
constexpr auto ddge::app::App<Addons_T...>::add_on(this Self_T&& self, Addon_T&& addon)
    -> App<Addons_T..., std::remove_cvref_t<Addon_T>>
{
    return App<Addons_T..., std::remove_cvref_t<Addon_T>>{
        static_cast<util::meta::forward_like_t<Addons_T, Self_T>>(self)...,
        std::forward<Addon_T>(addon)
    };
}

template <ddge::app::addon_c... Addons_T>
template <typename... UAddons_T>
    requires(std::constructible_from<Addons_T, UAddons_T &&> && ...)
constexpr ddge::app::App<Addons_T...>::App(UAddons_T&&... addons)
    : Addons_T{ std::forward<UAddons_T>(addons) }...
{}
