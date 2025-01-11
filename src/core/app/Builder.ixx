module;

#include <concepts>

#include <spdlog/spdlog.h>

export module core.app.Builder;

import :customization_c;
import :details;

import core.app.App;

namespace core::app {

export template <customization_c... Customizations_T>
class Builder : public BuilderBase<Customizations_T..., MonoCustomization> {
public:
    constexpr Builder() = default;

    template <customization_c NewCustomization_T, typename Self_T, typename... Args_T>
    constexpr auto customize(this Self_T&& self, Args_T&&... args)
        -> Builder<NewCustomization_T, Customizations_T...>;

    template <typename Self>
    constexpr auto build(this Self&& self);

private:
    friend old_builder_t<Customizations_T...>;

    template <typename OtherBuilder_T, typename... Args_T>
    constexpr explicit Builder(OtherBuilder_T&& other, Args_T&&... args);
};

export [[nodiscard]]
constexpr auto create() -> Builder<>;

export template <typename Builder_T>
concept builder_c =
    std::derived_from<std::remove_cvref_t<Builder_T>, BuilderBase<MonoCustomization>>;

export template <typename Customization_T>
concept customization_c = ::customization_c<Customization_T>;

export template <typename Builder_T, typename... Customizations_T>
concept customization_of_c =
    builder_c<Builder_T>
    && (std::derived_from<std::remove_cvref_t<Builder_T>, Customizations_T> && ...);

}   // namespace core::app

template <customization_c... Customizations_T>
template <typename OtherBuilder_T, typename... Args_T>
constexpr core::app::Builder<Customizations_T...>::Builder(
    OtherBuilder_T&& other,
    Args_T&&... args
)
    : BuilderBase<Customizations_T..., MonoCustomization>{
          std::forward<OtherBuilder_T>(other),
          std::forward<Args_T>(args)...
      }
{}

template <customization_c... Customizations_T>
template <customization_c NewCustomization_T, typename Self_T, typename... Args_T>
constexpr auto core::app::Builder<Customizations_T...>::customize(
    this Self_T&& self,
    Args_T&&... args
) -> Builder<NewCustomization_T, Customizations_T...>
{
    return Builder<NewCustomization_T, Customizations_T...>{
        std::forward<Self_T>(self), std::forward<Args_T>(args)...
    };
}

template <customization_c... Customizations_T>
template <typename Self>
constexpr auto core::app::Builder<Customizations_T...>::build(this Self&& self)
{
    SPDLOG_INFO("Building app...");
    return std::forward<Self>(self)
        .BuilderBase<Customizations_T..., MonoCustomization>::build(App<>{});
}

constexpr auto core::app::create() -> Builder<>
{
    return Builder<>{};
}
