import core;
import addons;
import extensions;
import utility;

import demo.Window;

using namespace extensions::scheduler::accessors::ecs;
using namespace extensions::scheduler::accessors::events;
using namespace extensions::scheduler::accessors::resources;
using namespace core::ecs::query_parameter_tags;

struct WindowClosed {};

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

constexpr static auto update_0 = [](const ClearEventsCommand clear_events) -> void {
    clear_events();
};

constexpr static auto update_1 = [](const Query<
                                     const Position,
                                     Without<Health>,
                                     With<EnemyTag>,
                                     Optional<Renderable>,
                                     Optional<const Collider>> entities) -> void {
    entities.for_each([](const Position,
                         util::OptionalRef<Renderable>,
                         util::OptionalRef<const Collider>) {});
};

constexpr static auto update_2 =
    [](const EventRecorder<WindowClosed> window_closed_event_recorder) {
        window_closed_event_recorder.record();
    };

constexpr static auto game_is_running =
    [](const EventReader<WindowClosed> window_closed_events) -> bool {
    return window_closed_events.count() == 0;
};

constexpr static auto run_game_loop = core::scheduler::loop_until(
    core::scheduler::start_as(update_0)   //
        .then(core::scheduler::group(update_1, update_2)),
    game_is_running
);

constexpr static auto shut_down = [](const Res<Window> window) { window->close(); };

auto main() -> int
{
    namespace dependency_providers = extensions::scheduler::dependency_providers;

    core::app::create()
        .extend_with(extensions::ResourceManager{})
        .use_resource(Window{})
        .extend_with(extensions::EventManager{})
        .register_event<WindowClosed>()
        .extend_with(extensions::AddonManager{})
        .use_addon(addons::ECS{})
        .extend_with(
            extensions::TaskRunner{ dependency_providers::ResourceManager{},
                                    dependency_providers::EventManager{},
                                    dependency_providers::ECS{} }
        )
        .run(
            core::scheduler::start_as(initialize)   //
                .then(run_game_loop)
                .then(shut_down)
        );
}
