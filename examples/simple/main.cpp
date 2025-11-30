#include <print>
#include <string>

import ddge.prelude;
import ddge.modules.ecs;
import ddge.modules.scheduler;
import ddge.utility.containers.OptionalRef;

import demo.Window;

using namespace ddge::scheduler::accessors;
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

constexpr static auto initialize =                                       //
    [](const Resource<Window> window, const Registry registry) -> void   //
{
    window->open();

    registry->create(Position{}, EnemyTag{}, Collider{ .message = "Hi! 👋" });
};

constexpr static auto process_events =              //
    [](const Processor& events_processor) -> void   //
{                                                   //
    events_processor.process_events();
};

[[nodiscard]]
auto update_world()
{
    return ddge::scheduler::query(
        +[](   //
             const ddge::ecs::ID            id,
             const Position                 position,
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
    [](const Recorder<WindowClosed>& window_closed_event_recorder) -> void   //
{                                                                            //
    window_closed_event_recorder.record();
};

constexpr static auto game_is_running =
    [](const Reader<WindowClosed>& window_closed_event_reader) -> bool   //
{                                                                        //
    return window_closed_event_reader.read().size() == 0;
};

[[nodiscard]]
auto run_game_loop()
{
    return ddge::scheduler::loop_until(
        ddge::scheduler::start_as(
            ddge::scheduler::group(
                update_world(),   //
                ddge::scheduler::as_task(record_window_events)
            )
        )
            .then(ddge::scheduler::as_task(process_events)),
        ddge::scheduler::as_task(game_is_running)
    );
}

constexpr static auto shut_down =               //
    [](const Resource<Window> window) -> void   //
{                                               //
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
            ddge::scheduler::start_as(ddge::scheduler::as_task(initialize))   //
                .then(run_game_loop())
                .then(ddge::scheduler::as_task(shut_down))
        );
}
