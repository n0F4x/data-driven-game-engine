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
    explicit AddonBuilder(std::in_place_t, Args_T... args);

protected:
    template <core::app::app_c App_T>
    auto operator()(App_T&& app) &&;

private:
    Addon_T m_addon;
};

export class AddonManager {
public:
    template <core::app::addon_c Addon_T, core::app::builder_c Self_T, typename... Args_T>
    auto use_addon(this Self_T&&, Args_T&&... args);
};

}   // namespace extensions

template <typename Addon_T>
template <typename... Args_T>
extensions::AddonBuilder<Addon_T>::AddonBuilder(std::in_place_t, Args_T... args)
    : m_addon(std::forward<Args_T>(args)...)
{}

template <typename Addon_T>
template <core::app::app_c App_T>
auto extensions::AddonBuilder<Addon_T>::operator()(App_T&& app) &&
{
    return std::forward<App_T>(app).template add_on<Addon_T>(std::move(m_addon));
}

template <core::app::addon_c Addon_T, core::app::builder_c Self_T, typename... Args_T>
auto extensions::AddonManager::use_addon(this Self_T&& self, Args_T&&... args)
{
    return std::forward<Self_T>(self).template extend_with<AddonBuilder<Addon_T>>(
        std::in_place, std::forward<Args_T>(args)...
    );
}
