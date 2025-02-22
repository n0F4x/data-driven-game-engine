module;

#include <concepts>
#include <utility>

export module core.app.App;

import :details;
import :addon_c;

namespace core::app {

export template <addon_c... Addons_T>
class App : public AppBase<Addons_T..., RootAddon> {
public:
    constexpr App() = default;

    template <typename OldApp_T, typename... Args_T>
        requires std::same_as<std::remove_cvref_t<OldApp_T>, old_app_t<Addons_T...>>
    constexpr explicit App(OldApp_T&& old_app, std::in_place_t, Args_T&&... args);

    template <addon_c NewAddon_T, typename Self_T, typename... Args_T>
    constexpr auto add_on(this Self_T&& self, Args_T&&... args)
        -> App<NewAddon_T, Addons_T...>;
};

export template <typename App_T>
concept app_c = std::derived_from<std::remove_cvref_t<App_T>, AppBase<RootAddon>>;

export template <typename Addon_T>
concept addon_c = ::addon_c<Addon_T>;

export template <typename App_T, typename... Addons_T>
concept has_addons_c = app_c<App_T>
                    && (std::derived_from<std::remove_cvref_t<App_T>, Addons_T> && ...);

}   // namespace core::app

template <addon_c... Addons_T>
template <typename OldApp_T, typename... Args_T>
    requires std::same_as<std::remove_cvref_t<OldApp_T>, old_app_t<Addons_T...>>
constexpr core::app::App<Addons_T...>::App(
    OldApp_T&& old_app,
    std::in_place_t,
    Args_T&&... args
)
    : AppBase<Addons_T..., RootAddon>{ std::forward<OldApp_T>(old_app),
                                       std::in_place,
                                       std::forward<Args_T>(args)... }
{}

template <addon_c... Addons_T>
template <addon_c NewAddon_T, typename Self_T, typename... Args_T>
constexpr auto core::app::App<Addons_T...>::add_on(this Self_T&& self, Args_T&&... args)
    -> App<NewAddon_T, Addons_T...>
{
    return App<NewAddon_T, Addons_T...>{ std::forward<Self_T>(self),
                                         std::in_place,
                                         std::forward<Args_T>(args)... };
}
