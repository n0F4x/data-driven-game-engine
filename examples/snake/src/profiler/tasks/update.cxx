module;

#include <chrono>

module snake.profiler.tasks.update;

import ddge.modules.scheduler.accessors.resources;
import ddge.modules.scheduler.primitives.as_task;

import snake.profiler.SecondTimer;
import snake.window.DisplayInfo;

using namespace ddge::scheduler::accessors;

auto profiler::tasks::update()
    -> ddge::scheduler::TaskBlueprint<void, ddge::scheduler::Cardinality::eSingle>
{
    return ddge::scheduler::as_task(
        [last_time = std::chrono::steady_clock::time_point{}, delta_update_count = 0u](
            const Resource<window::DisplayInfo> display_info,
            const Resource<SecondTimer>         second_timer
        ) mutable -> void {
            second_timer->update();
            ++delta_update_count;
            if (second_timer->delta_ticks() > 0) {
                display_info->average_ups = delta_update_count
                                          / second_timer->delta_ticks();
                delta_update_count = 0;
            }

            using namespace std::chrono_literals;

            const auto now{ std::chrono::steady_clock::now() };
            const auto delta{ now - last_time };
            last_time = now;
            const auto ups{ 1s / delta };

            display_info->ups = ups;
        }
    );
}
