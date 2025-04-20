import core;
import addons;
import extensions;
import utility;

import demo.Window;

using namespace extensions::scheduler::dependency_providers::tags;
using namespace core::ecs::query_parameter_tags;

using Position = float;
using Health   = int;

struct EnemyTag {};

struct Renderable {
    int hi{};
};

struct Collider {
    int hi{};
};

constexpr static auto initialize = [](const Res<Window> window) { window->open(); };

constexpr static auto update_0 = [](const ecs::Query<
                                     const Position,
                                     Without<Health>,
                                     With<EnemyTag>,
                                     Optional<Renderable>,
                                     Optional<const Collider>> entities) -> void {
    entities.for_each([](const Position,
                         util::OptionalRef<Renderable>,
                         util::OptionalRef<const Collider>) {});
};

constexpr static auto update_1 = [](const Res<Window> window) { window->close(); };

constexpr static auto game_is_running = [](const Res<const Window> window) -> bool {
    return window->is_open();
};

constexpr static auto run_game_loop = core::scheduler::loop_until(
    core::scheduler::group(update_0, update_1),
    game_is_running
);

auto main() -> int
{
    namespace dependency_providers = extensions::scheduler::dependency_providers;

    core::app::create()
        .extend_with(extensions::ResourceManager{})
        .use_resource(Window{})
        .extend_with(extensions::AddonManager{})
        .use_addon(addons::ECS{})
        .extend_with(
            extensions::TaskRunner{ dependency_providers::ECS{},
                                    dependency_providers::ResourceManager{} }
        )
        .run(
            core::scheduler::start_as(initialize)   //
                .then(run_game_loop)
        );
}
