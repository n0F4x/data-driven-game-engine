module;

#include <concepts>
#include <tuple>
#include <type_traits>
#include <utility>

export module core.app.Builder:details;

import :fwd;

import utility.meta.type_traits.integer_sequence.offset_integer_sequence;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.meta.type_traits.type_list.type_list_drop_front;
import utility.meta.type_traits.type_list.type_list_replace;
import utility.meta.type_traits.forward_like;
import utility.TypeList;


import core.app.App;

template <typename Extension_T, typename... RestOfExtensions_T>
class BuilderBase : public BuilderBase<RestOfExtensions_T...>, public Extension_T {
public:
    constexpr BuilderBase() = default;

    template <typename Base_T, typename... Args_T>
        requires std::constructible_from<BuilderBase<RestOfExtensions_T...>, Base_T>
    constexpr explicit BuilderBase(Base_T&& base, std::in_place_t, Args_T&&... args)
        : BuilderBase<RestOfExtensions_T...>{ std::forward<Base_T>(base) },
          Extension_T(std::forward<Args_T>(args)...)
    {}

    template <typename OldExtension_T, typename Self_T, typename Transform_T>
    constexpr auto swap_extension(this Self_T&& self, Transform_T&& transform_extension)
        -> util::meta::type_list_replace_t<
            BuilderBase,
            OldExtension_T,
            std::invoke_result_t<
                Transform_T,
                util::meta::forward_like_t<OldExtension_T, Self_T>>>
    {
        using Result = util::meta::type_list_replace_t<
            BuilderBase,
            OldExtension_T,
            std::invoke_result_t<
                Transform_T,
                util::meta::forward_like_t<OldExtension_T, Self_T>>>;

        if constexpr (std::is_same_v<Extension_T, OldExtension_T>) {
            return Result{
                static_cast<
                    util::meta::forward_like_t<BuilderBase<RestOfExtensions_T...>, Self_T>>(
                    std::forward<Self_T>(self)
                ),
                std::in_place,
                std::invoke(
                    std::forward<Transform_T>(transform_extension),
                    static_cast<util::meta::forward_like_t<Extension_T, Self_T>>(
                        std::forward<Self_T>(self)
                    )
                )
            };
        }
        else {
            return Result{ std::forward<Self_T>(self).BuilderBase<RestOfExtensions_T...>::
                               template swap_extension<OldExtension_T>(
                                   std::forward<Transform_T>(transform_extension)
                               ),
                           std::in_place,
                           static_cast<util::meta::forward_like_t<Extension_T, Self_T>>(
                               std::forward<Self_T>(self)
                           ) };
        }
    }

    template <typename Self_T, typename App_T>
        requires core::app::app_c<App_T>
    constexpr auto build(this Self_T&& self, App_T&& app)

    {
        if constexpr (class DummyAddon{}; requires(core::app::App<DummyAddon> dummy_app) {
                          core::app::app_c<
                              decltype(std::forward<Self_T>(self).Extension_T::operator()(
                                  std::forward_like<Self_T>(dummy_app)
                              ))>;
                      })
        {
            return std::forward<Self_T>(self).Extension_T::operator()(
                std::forward<Self_T>(self).BuilderBase<RestOfExtensions_T...>::build(
                    std::forward<App_T>(app)
                )
            );
        }
        else {
            return std::forward<Self_T>(self).BuilderBase<RestOfExtensions_T...>::build(
                std::forward<App_T>(app)
            );
        }
    }
};

class RootExtension {};

template <>
class BuilderBase<RootExtension> {
public:
    template <typename App_T>
    [[nodiscard]]
    constexpr auto build(App_T&& app) -> App_T
    {
        return std::forward<App_T>(app);
    }
};

template <typename... Extensions_T>
using old_builder_t =
    util::meta::type_list_drop_front_t<core::app::Builder<Extensions_T...>>;

template <typename Builder_T, typename OldExtension_T, typename NewExtension_T>
using swapped_builder_t =
    util::meta::type_list_replace_t<Builder_T, OldExtension_T, NewExtension_T>;

template <typename Transform_T, typename Self_T, typename OldExtension_T, typename... Extensions_T>
concept valid_swap_transform =
    requires {
        util::meta::type_list_contains_v<util::TypeList<Extensions_T...>, OldExtension_T>;
    }
    && extension_c<
        std::invoke_result_t<Transform_T, util::meta::forward_like_t<OldExtension_T, Self_T>>>
    && (!util::meta::type_list_contains_v<
        util::TypeList<Extensions_T...>,
        std::invoke_result_t<Transform_T, util::meta::forward_like_t<OldExtension_T, Self_T>>>);
