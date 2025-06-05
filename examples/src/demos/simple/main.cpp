#include <print>

import addons.ecs;

import core.app;
import core.ecs;
import core.scheduler;

import demo.Window;

import extensions.AddonManager;
import extensions.EventManager;
import extensions.ResourceManager;
import extensions.scheduler;
import extensions.TaskRunner;

import utility.containers.OptionalRef;

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

constexpr static auto initialize =                                               //
    [](const resources::Ref<Window> window, const ecs::RegistryRef registry) {   //
        window->open();

        registry->create(Position{}, EnemyTag{}, Collider{ .hi = 42 });
    };

constexpr static auto update_0 =                      //
    [](const events::Processor& events_processor) {   //
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
                         const util::OptionalRef<Renderable>,
                         const util::OptionalRef<const Collider> optional_collider) {
        if (optional_collider.has_value()) {
            std::println("Collider is {}", optional_collider->hi);
        }
    });
};

constexpr static auto update_2 =
    [](const events::Recorder<WindowClosed>& window_closed_event_recorder) {
        window_closed_event_recorder.record();
    };

constexpr static auto game_is_running =
    [](const events::Reader<WindowClosed>& window_closed_event_reader) {
        return window_closed_event_reader.read().size() == 0;
    };

constexpr static auto run_game_loop = core::scheduler::loop_until(
    core::scheduler::start_as(update_0)   //
        .then(core::scheduler::in_parallel(update_1, update_2)),
    game_is_running
);

constexpr static auto shut_down =               //
    [](const resources::Ref<Window> window) {   //
        window->close();
    };

auto main() -> int
{
    namespace argument_providers = extensions::scheduler::argument_providers;

    core::app::create()
        .extend_with(extensions::ResourceManager{})
        .use_resource(Window{})
        .extend_with(extensions::EventManager{})
        .register_event<WindowClosed>()
        .extend_with(extensions::addon_manager)
        .use_addon(addons::ECS{})
        .extend_with(
            extensions::TaskRunner{ argument_providers::resource_provider,
                                    argument_providers::event_provider,
                                    argument_providers::ecs }
        )
        .run(
            core::scheduler::start_as(initialize)   //
                .then(run_game_loop)
                .then(shut_down)
        );
}
