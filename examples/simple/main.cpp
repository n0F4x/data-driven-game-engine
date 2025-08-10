#include <print>
#include <string>

import ddge.prelude;
import ddge.modules.ecs;
import ddge.modules.execution;
import ddge.utility.containers.OptionalRef;

import demo.Window;

using namespace ddge::exec::accessors;
using namespace ddge::ecs::query_parameter_tags;

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
                         const ddge::util::OptionalRef<Renderable>,
                         const ddge::util::OptionalRef<const Collider> optional_collider) {
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

static const auto run_game_loop = ddge::exec::loop_until(
    ddge::exec::start_as(
        ddge::exec::group(
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
    ddge::app::create()
        .plug_in(ddge::plugins::Resources{})
        .insert_resource(Window{})
        .plug_in(ddge::plugins::Events{})
        .register_event<WindowClosed>()
        .plug_in(ddge::plugins::ECS{})
        .plug_in(ddge::plugins::Scheduler{})
        .run(
            ddge::exec::start_as(initialize)   //
                .then(run_game_loop)
                .then(shut_down)
        );
}
