module;

#include <utility>

export module ddge.modules.ecs:Plugin;

import ddge.modules.app;

import :Addon;

namespace ddge::ecs {

export class Plugin {
public:
    template <ddge::app::decays_to_app_c App_T>
    [[nodiscard]]
    constexpr auto build(App_T&& app) -> app::add_on_t<App_T, Addon>;
};

}   // namespace ddge::ecs

template <ddge::app::decays_to_app_c App_T>
constexpr auto ddge::ecs::Plugin::build(App_T&& app) -> app::add_on_t<App_T, Addon>
{
    return std::forward<App_T>(app).add_on(Addon{});
}
