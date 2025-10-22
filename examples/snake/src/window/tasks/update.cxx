module;

#include <utility>

#include <SFML/Window.hpp>

module snake.window.tasks.update;

import ddge.modules.time.FixedTimer;

import ddge.modules.execution.accessors.events.Recorder;
import ddge.modules.execution.accessors.resources;
import ddge.modules.execution.providers.EventProvider;
import ddge.modules.execution.providers.ResourceProvider;
import ddge.modules.execution.v2.primitives.force_on_main;

import snake.window.DisplayInfo;
import snake.window.DisplayTimer;
import snake.window.Settings;
import snake.window.Window;

using namespace ddge::exec::accessors;

auto window::update()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>
{
    return ddge::exec::v2::force_on_main(
        +[](   //
             const Resource<const Settings>    settings,
             const Resource<Window>            window,
             const Recorder<sf::Event>&        event_recorder,
             const Resource<DisplayTimer>      display_timer,
             const Resource<const DisplayInfo> display_info
         ) -> void   //
        {
            window->handleEvents([&event_recorder]<typename Event>(Event&& event) -> void {
                event_recorder.record(std::forward<Event>(event));
            });

            display_timer->update();

            window->setTitle(
                std::format(
                    "{}        "   //
                    "FPS: {:05d},  UPS: {:05d},    FPS(avg): {:05d},  UPS(avg): {:05d}",
                    settings->title,
                    display_info->fps,
                    display_info->ups,
                    display_info->average_fps,
                    display_info->average_ups
                )
            );
        }   //
    );
}
