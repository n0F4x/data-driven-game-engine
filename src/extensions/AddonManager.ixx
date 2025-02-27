module;

#include <functional>
#include <vector>

export module extensions.AddonManager;

import core.app.App;
import core.app.Builder;

namespace extensions {

template <typename Addon_T>
class AddonBuilder {
public:
    template <typename... Args_T>
    constexpr explicit AddonBuilder(std::in_place_t, Args_T... args);

protected:
    template <core::app::app_c App_T>
    constexpr auto operator()(App_T&& app) &&;

private:
    Addon_T m_addon;
};

export class AddonManager {
public:
    template <core::app::addon_c Addon_T, core::app::builder_c Self_T, typename... Args_T>
    constexpr auto use_addon(this Self_T&&, Args_T&&... args);

    template <typename Self_T, typename T>
        requires(requires { typename T::Addon; })
    constexpr auto use_addon(this Self_T&&, T&& arg);
};

}   // namespace extensions

template <typename Addon_T>
template <typename... Args_T>
constexpr extensions::AddonBuilder<Addon_T>::AddonBuilder(std::in_place_t, Args_T... args)
    : m_addon(std::forward<Args_T>(args)...)
{}

template <typename Addon_T>
template <core::app::app_c App_T>
constexpr auto extensions::AddonBuilder<Addon_T>::operator()(App_T&& app) &&
{
    return std::forward<App_T>(app).template add_on<Addon_T>(std::move(m_addon));
}

template <core::app::addon_c Addon_T, core::app::builder_c Self_T, typename... Args_T>
constexpr auto extensions::AddonManager::use_addon(this Self_T&& self, Args_T&&... args)
{
    return std::forward<Self_T>(self).template extend_with<AddonBuilder<Addon_T>>(
        std::in_place, std::forward<Args_T>(args)...
    );
}

template <typename Self_T, typename T>
    requires(requires { typename T::Addon; })
constexpr auto extensions::AddonManager::use_addon(this Self_T&& self, T&& arg)
{
    return []<typename AddonManager_T>(AddonManager_T&& addon_manager) {
        if constexpr (requires { typename T::AddonTag; }) {
            return std::forward<AddonManager_T>(addon_manager)
                .template use_addon<typename T::AddonTag>();
        }
        else {
            return std::forward<AddonManager_T>(addon_manager);
        }
    }(std::forward<Self_T>(self).template use_addon<typename T::Addon>(std::forward<T>(arg
           )));
}
