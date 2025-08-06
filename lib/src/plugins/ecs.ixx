module;

#include <utility>

export module plugins.ecs;

import app;

import addons.ECS;

namespace plugins {

export class ECS {
public:
    template <app::decays_to_app_c App_T>
    [[nodiscard]]
    constexpr auto build(App_T&& app) -> app::add_on_t<App_T, addons::ECS>;
};

}   // namespace plugins

template <app::decays_to_app_c App_T>
constexpr auto plugins::ECS::build(App_T&& app) -> app::add_on_t<App_T, addons::ECS>
{
    return std::forward<App_T>(app).add_on(addons::ECS{});
}
