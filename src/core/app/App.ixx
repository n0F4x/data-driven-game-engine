module;

#include <concepts>
#include <tuple>
#include <type_traits>

export module core.app.App;

namespace core::app {

template <typename Mixin_T, typename... RestOfMixins_T>
class AppBase : public AppBase<RestOfMixins_T...>, public Mixin_T {
protected:
    constexpr AppBase() = default;

    template <typename Base_T, typename... Args_T>
        requires std::convertible_to<Base_T, core::app::AppBase<RestOfMixins_T...>>
    constexpr explicit AppBase(Base_T&& base, Args_T&&... args);
};

class MonoMixin {};

template <>
class AppBase<MonoMixin> {};

export template <typename Mixin_T>
concept mixin_c = std::is_class_v<Mixin_T> && std::is_move_constructible_v<Mixin_T>;

export template <mixin_c Mixin_T = MonoMixin, mixin_c... RestOfMixins_T>
class App : public AppBase<Mixin_T, RestOfMixins_T...> {
public:
    constexpr App() = default;

    template <typename OtherApp_T, typename... Args_T>
        requires std::same_as<std::remove_cvref_t<OtherApp_T>, App<RestOfMixins_T...>>
    constexpr explicit App(OtherApp_T&& other, Args_T&&... args);

    template <mixin_c NewMixin_T, typename Self_T, typename... Args_T>
    constexpr auto mix(this Self_T&& self, Args_T&&... args)
        -> App<NewMixin_T, Mixin_T, RestOfMixins_T...>;
};

export template <typename App_T>
concept app_c = std::derived_from<App_T, AppBase<MonoMixin>>;

export template <typename App_T, typename... Mixins>
concept mixed_with_c = app_c<App_T> && (std::derived_from<App_T, Mixins> && ...);

}   // namespace core::app

template <typename Mixin_T, typename... RestOfMixins_T>
template <typename Base_T, typename... Args_T>
    requires std::convertible_to<Base_T, core::app::AppBase<RestOfMixins_T...>>
constexpr core::app::AppBase<Mixin_T, RestOfMixins_T...>::AppBase(
    Base_T&& base,
    Args_T&&... args
)
    : AppBase<RestOfMixins_T...>{ std::forward<Base_T>(base) },
      Mixin_T(std::forward<Args_T>(args)...)
{}

template <core::app::mixin_c Mixin_T, core::app::mixin_c... RestOfMixins_T>
template <typename OtherApp_T, typename... Args_T>
    requires std::
        same_as<std::remove_cvref_t<OtherApp_T>, core::app::App<RestOfMixins_T...>>
    constexpr core::app::App<Mixin_T, RestOfMixins_T...>::App(
        OtherApp_T&& other,
        Args_T&&... args
    )
    : AppBase<Mixin_T, RestOfMixins_T...>{ std::forward<OtherApp_T>(other),
                                           std::forward<Args_T>(args)... }
{}

template <core::app::mixin_c Mixin_T, core::app::mixin_c... RestOfMixins_T>
template <core::app::mixin_c NewMixin_T, typename Self_T, typename... Args_T>
constexpr auto core::app::App<Mixin_T, RestOfMixins_T...>::mix(
    this Self_T&& self,
    Args_T&&... args
) -> App<NewMixin_T, Mixin_T, RestOfMixins_T...>
{
    return App<NewMixin_T, Mixin_T, RestOfMixins_T...>{ std::forward<Self_T>(self),
                                                        std::forward<Args_T>(args)... };
}
