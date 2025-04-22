import core;
import addons;
import extensions;
import utility;

import demo.Window;

using namespace extensions::scheduler::accessors;
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

constexpr static auto initialize =              //
    [](const resources::Ref<Window> window) {   //
        window->open();
    };

constexpr static auto update_0 =                     //
    [](const events::Processor events_processor) {   //
        events_processor.process_events();
    };

constexpr static auto update_1 =   //
    [](const ecs::Query<
        const Position,
        Without<Health>,
        With<EnemyTag>,
        Optional<Renderable>,
        Optional<const Collider>> entities)   //
{
    entities.for_each([](const Position,
                         util::OptionalRef<Renderable>,
                         util::OptionalRef<const Collider>) {});
};

constexpr static auto update_2 =
    [](const events::Recorder<WindowClosed> window_closed_event_recorder) {
        window_closed_event_recorder.record();
    };

constexpr static auto game_is_running =
    [](const events::Reader<WindowClosed> window_closed_events) {
        return window_closed_events.count() == 0;
    };

constexpr static auto run_game_loop = core::scheduler::loop_until(
    core::scheduler::start_as(update_0)   //
        .then(core::scheduler::group(update_1, update_2)),
    game_is_running
);

constexpr static auto shut_down =               //
    [](const resources::Ref<Window> window) {   //
        window->close();
    };

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
