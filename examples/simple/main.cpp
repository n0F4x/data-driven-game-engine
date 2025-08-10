#include <print>
#include <string>

import prelude;

import modules.ecs;

import demo.Window;

import modules.scheduler;

import utility.containers.OptionalRef;

using namespace modules::scheduler::accessors;
using namespace modules::ecs::query_parameter_tags;

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

constexpr static auto initialize =                                                 //
    [](const resources::Resource<Window> window, const ecs::Registry registry) {   //
        window->open();

        registry->create(Position{}, EnemyTag{}, Collider{ .message = "Hi! 👋" });
    };

constexpr static auto process_events =                //
    [](const events::Processor& events_processor) {   //
        events_processor.process_events();
    };

constexpr static auto update_world =   //
    [](ecs::Query<
        const Position,
        Without<Health>,
        With<EnemyTag>,
        Optional<Renderable>,
        Optional<const Collider>>& entities)   //
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

static const auto run_game_loop = modules::scheduler::loop_until(
    modules::scheduler::start_as(
        modules::scheduler::group(
            update_world,   //
            record_window_events
        )
    )
        .then(process_events),
    game_is_running
);

constexpr static auto shut_down =                    //
    [](const resources::Resource<Window> window) {   //
        window->close();
    };

auto main() -> int
{
    app::create()
        .plug_in(plugins::Resources{})
        .insert_resource(Window{})
        .plug_in(plugins::Events{})
        .register_event<WindowClosed>()
        .plug_in(plugins::ECS{})
        .plug_in(plugins::Scheduler{})
        .run(
            modules::scheduler::start_as(initialize)   //
                .then(run_game_loop)
                .then(shut_down)
        );
}
