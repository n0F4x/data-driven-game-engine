module;

#include <utility>

export module plugins.ecs;

import app.decays_to_app_c;

import addons.ECS;

namespace plugins {

export struct ECSTag {};

export class ECS : public ECSTag {
public:
    template <app::decays_to_app_c App_T>
    [[nodiscard]]
    constexpr auto build(App_T&& app);
};

export inline constexpr ECS ecs;

}   // namespace plugins

template <app::decays_to_app_c App_T>
constexpr auto plugins::ECS::build(App_T&& app)
{
    return std::forward<App_T>(app).add_on(addons::ECS{});
}
