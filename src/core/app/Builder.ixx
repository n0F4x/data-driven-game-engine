module;

#include <concepts>
#include <tuple>
#include <type_traits>
#include <utility>

export module core.app.Builder;

import core.app.App;

namespace core::app {

template <typename Customization_T, typename... RestOfCustomizations_T>
class BuilderBase : public BuilderBase<RestOfCustomizations_T...>,
                    public Customization_T {
public:
    constexpr BuilderBase() = default;

    template <typename Base_T, typename... Args_T>
        requires std::
            convertible_to<Base_T, core::app::BuilderBase<RestOfCustomizations_T...>>
        constexpr explicit BuilderBase(Base_T&& base, Args_T&&... args);

    template <typename Self_T, typename App_T>
        requires app_c<std::remove_cvref_t<App_T>>
    constexpr auto build(this Self_T&& self, App_T&& app);
};

class MonoCustomization {};

template <>
class BuilderBase<MonoCustomization> {
public:
    template <typename App_T>
    [[nodiscard]]
    constexpr auto build(App_T&& app) -> App_T;
};

export template <typename Customization_T>
concept customization_c = std::is_class_v<Customization_T>
                       && std::is_move_constructible_v<Customization_T>;

export template <
    customization_c Customization_T = MonoCustomization,
    customization_c... RestOfCustomizations_T>
class Builder : public BuilderBase<Customization_T, RestOfCustomizations_T...> {
public:
    constexpr Builder() = default;

    template <typename OtherBuilder_T, typename... Args_T>
        requires std::same_as<
            std::remove_cvref_t<OtherBuilder_T>,
            Builder<RestOfCustomizations_T...>>
    constexpr explicit Builder(OtherBuilder_T&& other, Args_T&&... args);

    template <customization_c NewCustomization_T, typename Self_T, typename... Args_T>
    constexpr auto customize(this Self_T&& self, Args_T&&... args)
        -> Builder<NewCustomization_T, Customization_T, RestOfCustomizations_T...>;

    template <typename Self>
    constexpr auto build(this Self&& self);
};

export [[nodiscard]]
constexpr auto create() -> Builder<>;

export template <typename Builder_T>
concept builder_c = std::derived_from<Builder_T, BuilderBase<MonoCustomization>>;

export template <typename Builder_T, typename... Customizations>
concept customization_of_c = builder_c<Builder_T>
                          && (std::derived_from<Builder_T, Customizations> && ...);

}   // namespace core::app

template <typename Customization_T, typename... RestOfCustomizations_T>
template <typename Base_T, typename... Args_T>
    requires std::convertible_to<Base_T, core::app::BuilderBase<RestOfCustomizations_T...>>
constexpr core::app::BuilderBase<Customization_T, RestOfCustomizations_T...>::BuilderBase(
    Base_T&& base,
    Args_T&&... args
)
    : BuilderBase<RestOfCustomizations_T...>{ std::forward<Base_T>(base) },
      Customization_T(std::get<Args_T>(args)...)
{}

template <typename Customization_T, typename... RestOfCustomizations_T>
template <typename Self_T, typename App_T>
    requires core::app::app_c<std::remove_cvref_t<App_T>>
constexpr auto core::app::BuilderBase<Customization_T, RestOfCustomizations_T...>::build(
    this Self_T&& self,
    App_T&&       app
)
{
    if constexpr (requires {
                      std::forward<Self_T>(self).Customization_T::operator()(
                          std::forward<Self_T>(self)
                              .BuilderBase<RestOfCustomizations_T...>::build(
                                  std::forward<App_T>(app)
                              )
                      );
                  })
    {
        return std::forward<Self_T>(self).Customization_T::operator()(
            std::forward<Self_T>(self).BuilderBase<RestOfCustomizations_T...>::build(
                std::forward<App_T>(app)
            )
        );
    }
    else {
        return std::forward<Self_T>(self).BuilderBase<RestOfCustomizations_T...>::build(
            std::forward<App_T>(app)
        );
    }
}

template <typename App_T>
constexpr auto core::app::BuilderBase<core::app::MonoCustomization>::build(App_T&& app)
    -> App_T
{
    return std::forward<App_T>(app);
}

template <
    core::app::customization_c Customization_T,
    core::app::customization_c... RestOfCustomizations_T>
template <typename OtherBuilder_T, typename... Args_T>
    requires std::same_as<
        std::remove_cvref_t<OtherBuilder_T>,
        core::app::Builder<RestOfCustomizations_T...>>
constexpr core::app::Builder<Customization_T, RestOfCustomizations_T...>::Builder(
    OtherBuilder_T&& other,
    Args_T&&... args
)
    : BuilderBase<Customization_T, RestOfCustomizations_T...>{
          std::forward<OtherBuilder_T>(other),
          std::forward<Args_T>(args)...
      }
{}

template <
    core::app::customization_c Customization_T,
    core::app::customization_c... RestOfCustomizations_T>
template <core::app::customization_c NewCustomization_T, typename Self_T, typename... Args_T>
constexpr auto core::app::Builder<Customization_T, RestOfCustomizations_T...>::customize(
    this Self_T&& self,
    Args_T&&... args
) -> Builder<NewCustomization_T, Customization_T, RestOfCustomizations_T...>
{
    return Builder<NewCustomization_T, Customization_T, RestOfCustomizations_T...>{
        std::forward<Self_T>(self), std::forward<Args_T>(args)...
    };
}

template <core::app::customization_c Customization_T, core::app::customization_c... RestOfCustomizations_T>
template <typename Self>
constexpr auto core::app::Builder<Customization_T, RestOfCustomizations_T...>::build(
    this Self&& self
)
{
    return std::forward<Self>(self).BuilderBase<Customization_T, RestOfCustomizations_T...>::build(App<>{});
}

constexpr auto core::app::create() -> Builder<>
{
    return {};
}
