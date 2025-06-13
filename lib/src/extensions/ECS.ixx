module;

#include <utility>

export module extensions.ECS;

import core.app.decays_to_app_c;

import addons.ECS;

namespace extensions {

export struct ECSTag {};

export class ECS : public ECSTag {
public:
    template <core::app::decays_to_app_c App_T>
    [[nodiscard]]
    constexpr auto build(App_T&& app);
};

export inline constexpr ECS ecs;

}   // namespace extensions

template <core::app::decays_to_app_c App_T>
constexpr auto extensions::ECS::build(App_T&& app)
{
    return std::forward<App_T>(app).add_on(addons::ECS{});
}
