module;

#include <concepts>
#include <type_traits>
#include <utility>

export module modules.app.Builder;

import utility.meta.type_traits.forward_like;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.TypeList;

import modules.app.App;
import modules.app.app_c;
import modules.app.decays_to_plugin_c;
import modules.app.plugin_c;

namespace modules::app {

export template <plugin_c... Plugins_T>
class Builder : public Plugins_T... {
public:
    Builder() = default;

    template <typename Self_T, decays_to_plugin_c Plugin_T>
        requires(!util::meta::type_list_contains_v<util::TypeList<Plugins_T...>, Plugin_T>)
    [[nodiscard]]
    constexpr auto plug_in(this Self_T&&, Plugin_T&& plugin)
        -> Builder<Plugins_T..., std::remove_cvref_t<Plugin_T>>;

    template <typename Self_T>
    [[nodiscard]]
    constexpr auto build(this Self_T&&);

private:
    template <plugin_c...>
    friend class Builder;

    template <typename... UPlugins_T>
        requires(std::constructible_from<Plugins_T, UPlugins_T &&> && ...)
    constexpr explicit Builder(UPlugins_T&&... plugins);
};

}   // namespace app

template <modules::app::plugin_c... Plugins_T>
template <typename Self_T, modules::app::decays_to_plugin_c Plugin_T>
    requires(!util::meta::type_list_contains_v<util::TypeList<Plugins_T...>, Plugin_T>)
constexpr auto modules::app::Builder<Plugins_T...>::plug_in(this Self_T&& self, Plugin_T&& plugin)
    -> Builder<Plugins_T..., std::remove_cvref_t<Plugin_T>>
{
    return Builder<Plugins_T..., std::remove_cvref_t<Plugin_T>>{
        static_cast<util::meta::forward_like_t<Plugins_T, Self_T>>(self)...,
        std::forward<Plugin_T>(plugin)
    };
}

template <modules::app::plugin_c... Plugins_T>
template <typename Self_T>
constexpr auto modules::app::Builder<Plugins_T...>::build(this Self_T&& self)
{
    return [&self]<std::size_t index_T, typename App_T>(this auto&& func, App_T&& app) {
        if constexpr (index_T == sizeof...(Plugins_T)) {
            return std::forward<App_T>(app);
        }
        else if constexpr (requires {
                               {
                                   static_cast<
                                       util::meta::
                                           forward_like_t<Plugins_T...[index_T], Self_T>>(
                                       self
                                   )
                                       .build(std::forward<App_T>(app))
                               } -> app_c;
                           })
        {
            return func.template operator()<index_T + 1>(
                static_cast<util::meta::forward_like_t<Plugins_T...[index_T], Self_T>>(self
                )
                    .build(std::forward<App_T>(app))
            );
        }
        else {
            return func.template operator()<index_T + 1>(std::forward<App_T>(app));
        }
    }.template operator()<0>(App<>{});
}

template <modules::app::plugin_c... Plugins_T>
template <typename... UPlugins_T>
    requires(std::constructible_from<Plugins_T, UPlugins_T &&> && ...)
constexpr modules::app::Builder<Plugins_T...>::Builder(UPlugins_T&&... plugins)
    : Plugins_T{ std::forward<UPlugins_T>(plugins) }...
{}
