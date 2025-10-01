#include <print>
#include <string>

import ddge.prelude;
import ddge.modules.ecs;
import ddge.modules.execution;
import ddge.utility.containers.OptionalRef;

import demo.Window;

using namespace ddge::exec::accessors;
using namespace ddge::ecs::query_filter_tags;

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

[[nodiscard]]
auto update_world() -> ddge::exec::v2::TaskBuilder<void>
{
    return ddge::exec::v2::query(
        +[](   //
            const ddge::ecs::ID id,
            const Position position,
            const Optional<const Collider> optional_collider,
            With<EnemyTag>,
            Without<Renderable>
        ) -> void {
            if (optional_collider.has_value()) {
                std::println(
                    "Collider #{} at position {} says \"{}\"",
                    id.underlying(),
                    position,
                    optional_collider->message
                );
            }
        }
    );
}

constexpr static auto record_window_events =
    [](const events::Recorder<WindowClosed>& window_closed_event_recorder) {
        window_closed_event_recorder.record();
    };

constexpr static auto game_is_running =
    [](const events::Reader<WindowClosed>& window_closed_event_reader) -> bool   //
{                                                                                //
    return window_closed_event_reader.read().size() == 0;
};

[[nodiscard]]
auto run_game_loop() -> ddge::exec::v2::TaskBuilder<void>
{
    return ddge::exec::v2::loop_until(
        ddge::exec::v2::start_as(
            ddge::exec::v2::group(
                update_world(),   //
                ddge::exec::v2::as_task(record_window_events)
            )
        )
            .then(ddge::exec::v2::as_task(process_events)),
        ddge::exec::v2::as_task(game_is_running)
    );
}

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
            ddge::exec::v2::start_as(ddge::exec::v2::as_task(initialize))   //
                .then(run_game_loop())
                .then(ddge::exec::v2::as_task(shut_down))
        );
}
