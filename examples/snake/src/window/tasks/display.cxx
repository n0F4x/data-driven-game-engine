module;

#include <chrono>

module snake.window.tasks.display;

import ddge.modules.exec.accessors.resources;
import ddge.modules.exec.v2.primitives.as_task;

import snake.window.SecondTimer;
import snake.window.DisplayInfo;
import snake.window.Window;

using namespace ddge::exec::accessors;

auto window::tasks::display()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>
{
    return ddge::exec::v2::as_task(
        [last_time = std::chrono::steady_clock::time_point{}, delta_frame_count = 0u](
            const Resource<Window>      window,
            const Resource<DisplayInfo> display_info,
            const Resource<SecondTimer> second_timer
        ) mutable -> void   //
        {
            window->display();

            second_timer->update();
            ++delta_frame_count;
            if (second_timer->delta_ticks() > 0) {
                display_info->average_fps = delta_frame_count
                                          / second_timer->delta_ticks();
                delta_frame_count = 0;
            }

            using namespace std::chrono_literals;

            const auto now{ std::chrono::steady_clock::now() };
            const auto delta{ now - last_time };
            last_time = now;
            const auto fps{ 1s / delta };

            display_info->fps = fps;
        }
    );
}
