module;

#include <concepts>
#include <tuple>
#include <type_traits>
#include <utility>

#include "core/log/log.hpp"

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
class BasicBuilderBase : public BasicBuilderBase<RestOfExtensions_T...>,
                         public Extension_T {
public:
    constexpr BasicBuilderBase() = default;

    template <typename Base_T, typename... Args_T>
        requires std::constructible_from<BasicBuilderBase<RestOfExtensions_T...>, Base_T>
    constexpr explicit BasicBuilderBase(Base_T&& base, std::in_place_t, Args_T&&... args)
        : BasicBuilderBase<RestOfExtensions_T...>{ std::forward<Base_T>(base) },
          Extension_T(std::forward<Args_T>(args)...)
    {}

    template <typename OldExtension_T, typename Self_T, typename Transform_T>
    constexpr auto swap_extension(this Self_T&& self, Transform_T&& transform_extension)
        -> util::meta::type_list_replace_t<
            BasicBuilderBase,
            OldExtension_T,
            std::invoke_result_t<
                Transform_T,
                util::meta::forward_like_t<OldExtension_T, Self_T>>>
    {
        using Result = util::meta::type_list_replace_t<
            BasicBuilderBase,
            OldExtension_T,
            std::invoke_result_t<
                Transform_T,
                util::meta::forward_like_t<OldExtension_T, Self_T>>>;

        if constexpr (std::is_same_v<Extension_T, OldExtension_T>) {
            return Result{
                static_cast<
                    util::meta::
                        forward_like_t<BasicBuilderBase<RestOfExtensions_T...>, Self_T>>(
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
            return Result{
                std::forward<Self_T>(self).BasicBuilderBase<RestOfExtensions_T...>::
                    template swap_extension<OldExtension_T>(
                        std::forward<Transform_T>(transform_extension)
                    ),
                std::in_place,
                static_cast<util::meta::forward_like_t<Extension_T, Self_T>>(
                    std::forward<Self_T>(self)
                )
            };
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
                std::forward<Self_T>(self).BasicBuilderBase<RestOfExtensions_T...>::build(
                    std::forward<App_T>(app)
                )
            );
        }
        else {
            return std::forward<Self_T>(self)
                .BasicBuilderBase<RestOfExtensions_T...>::build(std::forward<App_T>(app));
        }
    }
};

class RootExtension {};

template <>
class BasicBuilderBase<RootExtension> {
public:
    template <typename App_T>
    [[nodiscard]]
    constexpr auto build(App_T&& app) -> App_T
    {
        return std::forward<App_T>(app);
    }
};

template <typename... Extensions_T>
using old_builder_t = util::meta::type_list_drop_front_t<BasicBuilder<Extensions_T...>>;

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

template <extension_c... Extensions_T>
class BasicBuilder : public BasicBuilderBase<Extensions_T..., RootExtension> {
    using Base = BasicBuilderBase<Extensions_T..., RootExtension>;

public:
    constexpr BasicBuilder() = default;

    constexpr explicit BasicBuilder(const Base& base) : Base{ base } {}

    constexpr explicit BasicBuilder(Base&& base) : Base{ std::move(base) } {}

    template <typename OldBuilder_T, typename... Args_T>
        requires std::
            same_as<std::remove_cvref_t<OldBuilder_T>, old_builder_t<Extensions_T...>>
        constexpr explicit BasicBuilder(
            OldBuilder_T&& old_builder,
            std::in_place_t,
            Args_T&&... args
        )
        : Base{ std::forward<OldBuilder_T>(old_builder),
                std::in_place,
                std::forward<Args_T>(args)... }
    {}

    template <extension_c NewExtension_T, typename Self_T, typename... Args_T>
    constexpr auto extend_with(this Self_T&& self, Args_T&&... args)
    {
        return []<typename Builder_T>(Builder_T&& builder) {
            if constexpr (requires {
                              typename NewExtension_T::ExtensionTag;
                              util::meta::type_list_contains_v<
                                  BasicBuilder<NewExtension_T, Extensions_T...>,
                                  typename NewExtension_T::ExtensionTag>;
                          })
            {
                return std::forward<Builder_T>(builder)
                    .template extend_with<typename NewExtension_T::ExtensionTag>();
            }
            else {
                return std::forward<Builder_T>(builder);
            }
        }(BasicBuilder<NewExtension_T, Extensions_T...>{ std::forward<Self_T>(self),
                                                         std::in_place,
                                                         std::forward<Args_T>(args)... });
    }

    template <
        typename OldExtension_T,
        typename Self_T,
        valid_swap_transform<Self_T, OldExtension_T, Extensions_T...> Transform_T>
    constexpr auto swap_extension(this Self_T&& self, Transform_T&& transform_extension)
        -> swapped_builder_t<
            Self_T,
            OldExtension_T,
            std::invoke_result_t<
                Transform_T,
                util::meta::forward_like_t<OldExtension_T, Self_T>>>
    {
        return swapped_builder_t<
            Self_T,
            OldExtension_T,
            std::invoke_result_t<
                Transform_T,
                util::meta::forward_like_t<OldExtension_T, Self_T>>>{
            std::forward<Self_T>(self).Base::template swap_extension<OldExtension_T>(
                std::forward<Transform_T>(transform_extension)
            )
        };
    }

    template <typename Self_T>
    constexpr auto build(this Self_T&& self)
    {
        if !consteval {
            ENGINE_LOG_INFO("Building app");
        }

        return std::forward<Self_T>(self).Base::build(core::app::App<>{});
    }
};
