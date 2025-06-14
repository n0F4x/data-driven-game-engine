module;

#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>

export module app.Builder;

import utility.meta.algorithms.apply;
import utility.meta.concepts.decays_to;
import utility.meta.concepts.specialization_of;
import utility.meta.type_traits.forward_like;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.meta.type_traits.type_list.type_list_replace;
import utility.TypeList;

import app.App;
import app.app_c;
import app.decays_to_plugin_c;
import app.plugin_c;

template <typename Transform_T, typename Builder_T, typename OldPlugin_T>
concept valid_swap_transform_c =
    requires {
        util::meta::type_list_contains_v<std::remove_cvref_t<Builder_T>, OldPlugin_T>;
    }
    && app::plugin_c<std::invoke_result_t<
        Transform_T,
        util::meta::forward_like_t<OldPlugin_T, Builder_T>>>
    && (!util::meta::type_list_contains_v<
        std::remove_cvref_t<Builder_T>,
        std::invoke_result_t<
            Transform_T,
            util::meta::forward_like_t<OldPlugin_T, Builder_T>>>);

namespace app {

export template <plugin_c... Plugins_T>
class Builder;

export template <
    typename OldPlugin_T,
    typename Builder_T,
    valid_swap_transform_c<Builder_T, OldPlugin_T> Transform_T>
    requires util::meta::specialization_of_c<std::remove_cvref_t<Builder_T>, Builder>
[[nodiscard]]
constexpr auto swap_plugin(Builder_T&& builder, Transform_T transform_plugin)
    -> util::meta::type_list_replace_t<
        std::remove_cvref_t<Builder_T>,
        OldPlugin_T,
        std::invoke_result_t<
            Transform_T,
            util::meta::forward_like_t<OldPlugin_T, Builder_T>>>;

export template <plugin_c... Plugins_T>
class Builder : public Plugins_T... {
public:
    Builder()
        requires(sizeof...(Plugins_T) == 0)
    = default;

    template <typename Self_T, decays_to_plugin_c Plugin_T>
        requires(!util::meta::
                     type_list_contains_v<util::TypeList<Plugins_T...>, Plugin_T>)
    [[nodiscard]]
    constexpr auto plug_in(this Self_T&&, Plugin_T&& plugin)
        -> Builder<Plugins_T..., std::remove_cvref_t<Plugin_T>>;

    template <typename Self_T>
    [[nodiscard]]
    constexpr auto build(this Self_T&&);

    template <
        typename OldPlugin_T,
        typename Builder_T,
        valid_swap_transform_c<Builder_T, OldPlugin_T> Transform_T>
        requires util::meta::
            specialization_of_c<std::remove_cvref_t<Builder_T>, ::app::Builder>
        friend constexpr auto
        swap_plugin(Builder_T&& builder, Transform_T transform_plugin)
            -> util::meta::type_list_replace_t<
                std::remove_cvref_t<Builder_T>,
                OldPlugin_T,
                std::invoke_result_t<
                    Transform_T,
                    util::meta::forward_like_t<OldPlugin_T, Builder_T>>>;

private:
    template <plugin_c...>
    friend class Builder;

    template <typename... UPlugins_T>
        requires(std::constructible_from<Plugins_T, UPlugins_T &&> && ...)
    constexpr explicit Builder(UPlugins_T&&... plugins);
};

}   // namespace app

template <
    typename OldPlugin_T,
    typename Builder_T,
    valid_swap_transform_c<Builder_T, OldPlugin_T> Transform_T>
    requires util::meta::
        specialization_of_c<std::remove_cvref_t<Builder_T>, app::Builder>
    constexpr auto
    app::swap_plugin(Builder_T&& builder, Transform_T transform_plugin)
        -> util::meta::type_list_replace_t<
            std::remove_cvref_t<Builder_T>,
            OldPlugin_T,
            std::invoke_result_t<
                Transform_T,
                util::meta::forward_like_t<OldPlugin_T, Builder_T>>>
{
    using Result = util::meta::type_list_replace_t<
        std::remove_cvref_t<Builder_T>,
        OldPlugin_T,
        std::invoke_result_t<
            Transform_T,
            util::meta::forward_like_t<OldPlugin_T, Builder_T>>>;

    return util::meta::apply<std::remove_cvref_t<Builder_T>>(
        [&builder, &transform_plugin]<typename... Plugins_T> -> Result {
            return Result{ [&builder, &transform_plugin] {
                if constexpr (std::is_same_v<Plugins_T, OldPlugin_T>) {
                    return std::invoke(
                        transform_plugin,
                        static_cast<util::meta::forward_like_t<Plugins_T, Builder_T>>(
                            builder
                        )
                    );
                }
                else {
                    std::ignore = transform_plugin;
                    return static_cast<util::meta::forward_like_t<Plugins_T, Builder_T>>(
                        builder
                    );
                }
            }()... };
        }
    );
}

template <app::plugin_c... Plugins_T>
template <typename Self_T, app::decays_to_plugin_c Plugin_T>
    requires(!util::meta::
                 type_list_contains_v<util::TypeList<Plugins_T...>, Plugin_T>)
constexpr auto app::Builder<Plugins_T...>::plug_in(
    this Self_T&& self,
    Plugin_T&& plugin
) -> Builder<Plugins_T..., std::remove_cvref_t<Plugin_T>>
{
    return Builder<Plugins_T..., std::remove_cvref_t<Plugin_T>>{
        static_cast<util::meta::forward_like_t<Plugins_T, Self_T>>(self)...,
        std::forward<Plugin_T>(plugin)
    };
}

template <app::plugin_c... Plugins_T>
template <typename Self_T>
constexpr auto app::Builder<Plugins_T...>::build(this Self_T&& self)
{
    return [&self]<size_t index_T, typename App_T>(this auto&& func, App_T&& app) {
        if constexpr (index_T == sizeof...(Plugins_T)) {
            return std::forward<App_T>(app);
        }
        else if constexpr (requires {
                               {
                                   static_cast<util::meta::forward_like_t<
                                       Plugins_T...[index_T],
                                       Self_T>>(self)
                                       .build(std::forward<App_T>(app))
                               } -> app_c;
                           })
        {
            return func.template operator()<index_T + 1>(
                static_cast<util::meta::forward_like_t<Plugins_T...[index_T], Self_T>>(
                    self
                )
                    .build(std::forward<App_T>(app))
            );
        }
        else {
            return func.template operator()<index_T + 1>(std::forward<App_T>(app));
        }
    }.template operator()<0>(App<>{});
}

template <app::plugin_c... Plugins_T>
template <typename... UPlugins_T>
    requires(std::constructible_from<Plugins_T, UPlugins_T &&> && ...)
constexpr app::Builder<Plugins_T...>::Builder(UPlugins_T&&... plugins)
    : Plugins_T{ std::forward<UPlugins_T>(plugins) }...
{}
