module;

#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>

export module core.app.Builder;

import utility.meta.algorithms.apply;
import utility.meta.concepts.decays_to;
import utility.meta.concepts.specialization_of;
import utility.meta.type_traits.forward_like;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.meta.type_traits.type_list.type_list_replace;
import utility.TypeList;

import core.app.App;
import core.app.app_c;
import core.app.decays_to_extension_c;
import core.app.extension_c;

template <typename Transform_T, typename Builder_T, typename OldExtension_T>
concept valid_swap_transform_c =
    requires {
        util::meta::type_list_contains_v<std::remove_cvref_t<Builder_T>, OldExtension_T>;
    }
    && core::app::extension_c<std::invoke_result_t<
        Transform_T,
        util::meta::forward_like_t<OldExtension_T, Builder_T>>>
    && (!util::meta::type_list_contains_v<
        std::remove_cvref_t<Builder_T>,
        std::invoke_result_t<
            Transform_T,
            util::meta::forward_like_t<OldExtension_T, Builder_T>>>);

namespace core::app {

export template <extension_c... Extensions_T>
class Builder;

export template <
    typename OldExtension_T,
    typename Builder_T,
    valid_swap_transform_c<Builder_T, OldExtension_T> Transform_T>
    requires util::meta::specialization_of_c<std::remove_cvref_t<Builder_T>, Builder>
[[nodiscard]]
constexpr auto swap_extension(Builder_T&& builder, Transform_T transform_extension)
    -> util::meta::type_list_replace_t<
        std::remove_cvref_t<Builder_T>,
        OldExtension_T,
        std::invoke_result_t<
            Transform_T,
            util::meta::forward_like_t<OldExtension_T, Builder_T>>>;

export template <extension_c... Extensions_T>
class Builder : public Extensions_T... {
public:
    Builder()
        requires(sizeof...(Extensions_T) == 0)
    = default;

    template <typename Self_T, decays_to_extension_c Extension_T>
        requires(!util::meta::
                     type_list_contains_v<util::TypeList<Extensions_T...>, Extension_T>)
    [[nodiscard]]
    constexpr auto extend_with(this Self_T&&, Extension_T&& extension)
        -> Builder<Extensions_T..., std::remove_cvref_t<Extension_T>>;

    template <typename Self_T>
    [[nodiscard]]
    constexpr auto build(this Self_T&&);

    template <
        typename OldExtension_T,
        typename Builder_T,
        valid_swap_transform_c<Builder_T, OldExtension_T> Transform_T>
        requires util::meta::
            specialization_of_c<std::remove_cvref_t<Builder_T>, ::core::app::Builder>
        friend constexpr auto
        swap_extension(Builder_T&& builder, Transform_T transform_extension)
            -> util::meta::type_list_replace_t<
                std::remove_cvref_t<Builder_T>,
                OldExtension_T,
                std::invoke_result_t<
                    Transform_T,
                    util::meta::forward_like_t<OldExtension_T, Builder_T>>>;

private:
    template <extension_c...>
    friend class Builder;

    template <typename... UExtensions_T>
        requires(std::constructible_from<Extensions_T, UExtensions_T &&> && ...)
    constexpr explicit Builder(UExtensions_T&&... extensions);
};

}   // namespace core::app

template <
    typename OldExtension_T,
    typename Builder_T,
    valid_swap_transform_c<Builder_T, OldExtension_T> Transform_T>
    requires util::meta::
        specialization_of_c<std::remove_cvref_t<Builder_T>, core::app::Builder>
    constexpr auto
    core::app::swap_extension(Builder_T&& builder, Transform_T transform_extension)
        -> util::meta::type_list_replace_t<
            std::remove_cvref_t<Builder_T>,
            OldExtension_T,
            std::invoke_result_t<
                Transform_T,
                util::meta::forward_like_t<OldExtension_T, Builder_T>>>
{
    using Result = util::meta::type_list_replace_t<
        std::remove_cvref_t<Builder_T>,
        OldExtension_T,
        std::invoke_result_t<
            Transform_T,
            util::meta::forward_like_t<OldExtension_T, Builder_T>>>;

    return util::meta::apply<std::remove_cvref_t<Builder_T>>(
        [&builder, &transform_extension]<typename... Extensions_T> -> Result {
            return Result{ [&builder, &transform_extension] {
                if constexpr (std::is_same_v<Extensions_T, OldExtension_T>) {
                    return std::invoke(
                        transform_extension,
                        static_cast<util::meta::forward_like_t<Extensions_T, Builder_T>>(
                            builder
                        )
                    );
                }
                else {
                    std::ignore = transform_extension;
                    return static_cast<util::meta::forward_like_t<Extensions_T, Builder_T>>(
                        builder
                    );
                }
            }()... };
        }
    );
}

template <core::app::extension_c... Extensions_T>
template <typename Self_T, core::app::decays_to_extension_c Extension_T>
    requires(!util::meta::
                 type_list_contains_v<util::TypeList<Extensions_T...>, Extension_T>)
constexpr auto core::app::Builder<Extensions_T...>::extend_with(
    this Self_T&& self,
    Extension_T&& extension
) -> Builder<Extensions_T..., std::remove_cvref_t<Extension_T>>
{
    return Builder<Extensions_T..., std::remove_cvref_t<Extension_T>>{
        static_cast<util::meta::forward_like_t<Extensions_T, Self_T>>(self)...,
        std::forward<Extension_T>(extension)
    };
}

template <core::app::extension_c... Extensions_T>
template <typename Self_T>
constexpr auto core::app::Builder<Extensions_T...>::build(this Self_T&& self)
{
    return [&self]<size_t index_T, typename App_T>(this auto&& func, App_T&& app) {
        if constexpr (index_T == sizeof...(Extensions_T)) {
            return std::forward<App_T>(app);
        }
        else if constexpr (requires {
                               {
                                   static_cast<util::meta::forward_like_t<
                                       Extensions_T...[index_T],
                                       Self_T>>(self)
                                       .build(std::forward<App_T>(app))
                               } -> app_c;
                           })
        {
            return func.template operator()<index_T + 1>(
                static_cast<util::meta::forward_like_t<Extensions_T...[index_T], Self_T>>(
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

template <core::app::extension_c... Extensions_T>
template <typename... UExtensions_T>
    requires(std::constructible_from<Extensions_T, UExtensions_T &&> && ...)
constexpr core::app::Builder<Extensions_T...>::Builder(UExtensions_T&&... extensions)
    : Extensions_T{ std::forward<UExtensions_T>(extensions) }...
{}
