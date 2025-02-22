module;

#include <concepts>

#include "core/log/log.hpp"

export module core.app.Builder;

import :extension_c;
import :details;

import core.app.App;

namespace core::app {

export template <extension_c... Extensions_T>
class Builder : public BuilderBase<Extensions_T..., RootExtension> {
    using Base = BuilderBase<Extensions_T..., RootExtension>;

public:
    constexpr Builder() = default;

    constexpr explicit Builder(const Base& base);
    constexpr explicit Builder(Base&& base);

    template <typename OldBuilder_T, typename... Args_T>
        requires std::
            same_as<std::remove_cvref_t<OldBuilder_T>, old_builder_t<Extensions_T...>>
        constexpr explicit Builder(
            OldBuilder_T&& old_builder,
            std::in_place_t,
            Args_T&&... args
        );

    template <extension_c NewExtension_T, typename Self_T, typename... Args_T>
    constexpr auto extend_with(this Self_T&& self, Args_T&&... args)
        -> Builder<NewExtension_T, Extensions_T...>;

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
                util::meta::forward_like_t<OldExtension_T, Self_T>>>;

    template <typename Self>
    constexpr auto build(this Self&& self);
};

export [[nodiscard]]
constexpr auto create() -> Builder<>;

export template <typename Builder_T>
concept builder_c =
    std::derived_from<std::remove_cvref_t<Builder_T>, BuilderBase<RootExtension>>;

export template <typename Extension_T>
concept extension_c = ::extension_c<Extension_T>;

export template <typename Builder_T, typename... Extensions_T>
concept extended_with_c = builder_c<Builder_T>
                       && (std::derived_from<std::remove_cvref_t<Builder_T>, Extensions_T>
                           && ...);

}   // namespace core::app

template <extension_c... Extensions_T>
constexpr core::app::Builder<Extensions_T...>::Builder(const Base& base) : Base{ base }
{}

template <extension_c... Extensions_T>
constexpr core::app::Builder<Extensions_T...>::Builder(Base&& base)
    : Base{ std::move(base) }
{}

template <extension_c... Extensions_T>
template <typename OldBuilder_T, typename... Args_T>
    requires std::same_as<std::remove_cvref_t<OldBuilder_T>, old_builder_t<Extensions_T...>>
constexpr core::app::Builder<Extensions_T...>::Builder(
    OldBuilder_T&& old_builder,
    std::in_place_t,
    Args_T&&... args
)
    : Base{ std::forward<OldBuilder_T>(old_builder),
            std::in_place,
            std::forward<Args_T>(args)... }
{}

template <extension_c... Extensions_T>
template <extension_c NewExtension_T, typename Self_T, typename... Args_T>
constexpr auto
    core::app::Builder<Extensions_T...>::extend_with(this Self_T&& self, Args_T&&... args)
        -> Builder<NewExtension_T, Extensions_T...>
{
    return Builder<NewExtension_T, Extensions_T...>{ std::forward<Self_T>(self),
                                                     std::in_place,
                                                     std::forward<Args_T>(args)... };
}

template <extension_c... Extensions_T>
template <
    typename OldExtension_T,
    typename Self_T,
    valid_swap_transform<Self_T, OldExtension_T, Extensions_T...> Transform_T>
constexpr auto core::app::Builder<Extensions_T...>::swap_extension(
    this Self_T&& self,
    Transform_T&& transform_extension
)
    -> swapped_builder_t<
        Self_T,
        OldExtension_T,
        std::invoke_result_t<Transform_T, util::meta::forward_like_t<OldExtension_T, Self_T>>>
{
    return swapped_builder_t<
        Self_T,
        OldExtension_T,
        std::invoke_result_t<Transform_T, util::meta::forward_like_t<OldExtension_T, Self_T>>>{
        std::forward<Self_T>(self).Base::template swap_extension<OldExtension_T>(
            std::forward<Transform_T>(transform_extension)
        )
    };
}

template <extension_c... Extensions_T>
template <typename Self_T>
constexpr auto core::app::Builder<Extensions_T...>::build(this Self_T&& self)
{
    ENGINE_LOG_INFO("Building app");
    return std::forward<Self_T>(self).Base::build(App<>{});
}

constexpr auto core::app::create() -> Builder<>
{
    return Builder<>{};
}

module :private;

#ifdef ENGINE_ENABLE_TESTS

static_assert(
    [] {
        std::ignore = core::app::create();

        return true;
    }(),
    "create test failed"
);

static_assert(
    [] {
        struct Dummy1 {};
        struct Dummy2 {};

        std::ignore = core::app::create().extend_with<Dummy1>().extend_with<Dummy2>();

        return true;
    }(),
    "extend_with test failed"
);

static_assert(
    [] {
        struct Dummy1 {};
        struct Dummy2 {};

        std::ignore = core::app::create().extend_with<Dummy1>().swap_extension<Dummy1>(
            [](Dummy1) { return Dummy2{}; }
        );

        return true;
    }(),
    "swap test failed"
);

#endif
