module;

#include <concepts>

#include <spdlog/spdlog.h>

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

    template <extension_c NewExtension_T, typename Self_T, typename... Args_T>
    constexpr auto extend_with(this Self_T&& self, Args_T&&... args)
        -> Builder<NewExtension_T, Extensions_T...>;

    template <typename Self>
    constexpr auto build(this Self&& self);

private:
    friend old_builder_t<Extensions_T...>;

    template <typename OtherBuilder_T, typename... Args_T>
    constexpr explicit Builder(OtherBuilder_T&& other, std::in_place_t, Args_T&&... args);
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
template <typename OtherBuilder_T, typename... Args_T>
constexpr core::app::Builder<Extensions_T...>::Builder(
    OtherBuilder_T&& other,
    std::in_place_t,
    Args_T&&... args
)
    : Base{ std::forward<OtherBuilder_T>(other),
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
template <typename Self>
constexpr auto core::app::Builder<Extensions_T...>::build(this Self&& self)
{
    SPDLOG_INFO("Building app");
    return std::forward<Self>(self).Base::build(App<>{});
}

constexpr auto core::app::create() -> Builder<>
{
    return Builder<>{};
}
