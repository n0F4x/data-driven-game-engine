module;

#include <utility>

export module modules.ecs:Plugin;

import modules.app;

import :Addon;

namespace modules::ecs {

export class Plugin {
public:
    template <modules::app::decays_to_app_c App_T>
    [[nodiscard]]
    constexpr auto build(App_T&& app) -> app::add_on_t<App_T, Addon>;
};

}   // namespace modules::ecs

template <modules::app::decays_to_app_c App_T>
constexpr auto modules::ecs::Plugin::build(App_T&& app) -> app::add_on_t<App_T, Addon>
{
    return std::forward<App_T>(app).add_on(Addon{});
}
