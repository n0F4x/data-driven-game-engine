module;

#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>

export module plugins.addons;

import app.addon_c;
import app.decays_to_addon_c;
import app.decays_to_app_c;
import app.decays_to_builder_c;

namespace plugins {

template <typename T>
concept addon_maker =
    app::addon_c<std::invoke_result_t<std::add_rvalue_reference_t<T>>>;

template <typename T>
concept decays_to_addon_maker = addon_maker<std::decay_t<T>>;

template <addon_maker AddonMaker_T>
class AddonBuilder {
public:
    template <typename UAddonMaker_T>
        requires std::constructible_from<AddonMaker_T, UAddonMaker_T&&>
    constexpr explicit AddonBuilder(UAddonMaker_T&& injection);

    template <app::decays_to_app_c App_T>
    constexpr auto build(App_T&& app) &&;

private:
    AddonMaker_T m_injection;
};

export class Addons {
public:
    template <app::decays_to_builder_c Self_T, app::decays_to_addon_c Addon_T>
    constexpr auto use_addon(this Self_T&&, Addon_T&& addon);

    template <app::decays_to_builder_c Self_T, decays_to_addon_maker AddonMaker_T>
    constexpr auto inject_addon(this Self_T&&, AddonMaker_T&& addon);
};

export inline constexpr Addons addons;

}   // namespace plugins

template <plugins::addon_maker AddonMaker_T>
template <typename UAddonMaker_T>
    requires std::constructible_from<AddonMaker_T, UAddonMaker_T&&>
constexpr plugins::AddonBuilder<AddonMaker_T>::AddonBuilder(UAddonMaker_T&& injection)
    : m_injection{ std::forward<UAddonMaker_T>(injection) }
{}

template <plugins::addon_maker AddonMaker_T>
template <app::decays_to_app_c App_T>
constexpr auto plugins::AddonBuilder<AddonMaker_T>::build(App_T&& app) &&
{
    return std::forward<App_T>(app).add_on(std::invoke(std::move(m_injection)));
}

template <app::decays_to_builder_c Self_T, app::decays_to_addon_c Addon_T>
constexpr auto plugins::Addons::use_addon(this Self_T&& self, Addon_T&& addon)
{
    struct AddonMaker {
        constexpr auto operator()() && -> std::decay_t<Addon_T>
        {
            return std::move(x_addon);
        }

        std::decay_t<Addon_T> x_addon;
    };

    return std::forward<Self_T>(self).plug_in(
        AddonBuilder<AddonMaker>{ AddonMaker{ .x_addon = std::forward<Addon_T>(addon) } }
    );
}

template <app::decays_to_builder_c Self_T, plugins::decays_to_addon_maker AddonMaker_T>
constexpr auto plugins::Addons::inject_addon(this Self_T&& self, AddonMaker_T&& addon)
{
    return std::forward<Self_T>(self).plug_in(
        AddonBuilder<AddonMaker_T>{ std::forward<AddonMaker_T>(addon) }
    );
}
