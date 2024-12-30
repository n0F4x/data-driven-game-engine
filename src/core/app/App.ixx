module;

#include <concepts>

export module core.app.App;

import :details;
import :mixin_c;

import utility.meta.offset;

namespace core::app {

export template <mixin_c... Mixins_T>
class App : public AppBase<Mixins_T..., MonoMixin> {
public:
    constexpr App() = default;

    template <mixin_c NewMixin_T, typename Self_T, typename... Args_T>
    constexpr auto mix(this Self_T&& self, Args_T&&... args)
        -> App<NewMixin_T, Mixins_T...>;

private:
    friend old_app_t<Mixins_T...>;

    template <typename OldApp_T, typename... Args_T>
    constexpr explicit App(OldApp_T&& old_app, Args_T&&... args);
};

export template <typename App_T>
concept app_c = std::derived_from<std::remove_cvref_t<App_T>, AppBase<MonoMixin>>;

export template <typename Mixin_T>
concept mixin_c = ::mixin_c<Mixin_T>;

export template <typename App_T, typename... Mixins>
concept mixed_with_c = app_c<App_T>
                    && (std::derived_from<std::remove_cvref_t<App_T>, Mixins> && ...);

}   // namespace core::app

template <mixin_c... Mixins_T>
template <mixin_c NewMixin_T, typename Self_T, typename... Args_T>
constexpr auto core::app::App<Mixins_T...>::mix(this Self_T&& self, Args_T&&... args)
    -> App<NewMixin_T, Mixins_T...>
{
    return App<NewMixin_T, Mixins_T...>{ std::forward<Self_T>(self),
                                         std::forward<Args_T>(args)... };
}

template <mixin_c... Mixins>
template <typename OldApp_T, typename... Args_T>
constexpr core::app::App<Mixins...>::App(OldApp_T&& old_app, Args_T&&... args)
    : AppBase<Mixins..., MonoMixin>{ std::forward<OldApp_T>(old_app),
                                     std::forward<Args_T>(args)... }
{}
