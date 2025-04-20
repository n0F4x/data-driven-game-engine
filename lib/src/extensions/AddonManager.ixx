module;

#include <concepts>
#include <functional>
#include <vector>

export module extensions.AddonManager;

import core.app.addon_c;
import core.app.decays_to_addon_c;
import core.app.decays_to_app_c;
import core.app.decays_to_builder_c;

namespace extensions {

template <typename Addon_T>
class AddonBuilder {
public:
    template <typename UAddon_T>
        requires std::constructible_from<Addon_T, UAddon_T&&>
    constexpr explicit AddonBuilder(UAddon_T&& addon);

    template <core::app::decays_to_app_c App_T>
    constexpr auto build(App_T&& app) &&;

private:
    Addon_T m_addon;
};

export class AddonManager {
public:
    template <core::app::decays_to_builder_c Self_T, core::app::decays_to_addon_c Addon_T>
    constexpr auto use_addon(this Self_T&&, Addon_T&& addon);
};

}   // namespace extensions

template <typename Addon_T>
template <typename UAddon_T>
    requires std::constructible_from<Addon_T, UAddon_T&&>
constexpr extensions::AddonBuilder<Addon_T>::AddonBuilder(UAddon_T&& addon)
    : m_addon{ std::forward<UAddon_T>(addon) }
{}

template <typename Addon_T>
template <core::app::decays_to_app_c App_T>
constexpr auto extensions::AddonBuilder<Addon_T>::build(App_T&& app) &&
{
    return std::forward<App_T>(app).add_on(std::move(m_addon));
}

template <core::app::decays_to_builder_c Self_T, core::app::decays_to_addon_c Addon_T>
constexpr auto extensions::AddonManager::use_addon(this Self_T&& self, Addon_T&& addon)
{
    return std::forward<Self_T>(self).extend_with(
        AddonBuilder<Addon_T>{ std::forward<Addon_T>(addon) }
    );
}
