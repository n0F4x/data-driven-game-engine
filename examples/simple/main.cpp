#include <print>
#include <string>

import app;

import core.ecs;
import core.scheduler;

import demo.Window;

import extensions.scheduler;

import plugins.ecs;
import plugins.events;
import plugins.resources;
import plugins.scheduler;

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
    std::string message{};
};

constexpr static auto initialize =                                               //
    [](const resources::Resource<Window> window, const ecs::Registry registry) {   //
        window->open();

        registry->create(Position{}, EnemyTag{}, Collider{ .message = "Hi! 👋" });
    };

constexpr static auto process_events =                //
    [](const events::Processor& events_processor) {   //
        events_processor.process_events();
    };

constexpr static auto update_world =   //
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
            std::println("Collider says \"{}\"", optional_collider->message);
        }
    });
};

constexpr static auto record_window_events =
    [](const events::Recorder<WindowClosed>& window_closed_event_recorder) {
        window_closed_event_recorder.record();
    };

constexpr static auto game_is_running =
    [](const events::Reader<WindowClosed>& window_closed_event_reader) {
        return window_closed_event_reader.read().size() == 0;
    };

constexpr static auto run_game_loop = core::scheduler::loop_until(
    core::scheduler::start_as(
        core::scheduler::group(
            update_world,   //
            record_window_events
        )
    )
        .then(process_events),
    game_is_running
);

constexpr static auto shut_down =               //
    [](const resources::Resource<Window> window) {   //
        window->close();
    };

auto main() -> int
{
    app::create()
        .plug_in(plugins::resources)
        .insert_resource(Window{})
        .plug_in(plugins::events)
        .register_event<WindowClosed>()
        .plug_in(plugins::ecs)
        .plug_in(plugins::scheduler)
        .run(
            core::scheduler::start_as(initialize)   //
                .then(run_game_loop)
                .then(shut_down)
        );
}
