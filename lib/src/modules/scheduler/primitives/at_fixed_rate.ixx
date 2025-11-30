module;

#include <utility>

export module ddge.modules.scheduler.primitives.at_fixed_rate;

import ddge.modules.time.FixedTimer;
import ddge.modules.time.VariableTimer;

import ddge.modules.scheduler.accessors.resources;
import ddge.modules.scheduler.convertible_to_TaskBlueprint_c;
import ddge.modules.scheduler.Cardinality;
import ddge.modules.scheduler.primitives.as_task;
import ddge.modules.scheduler.primitives.repeat;
import ddge.modules.scheduler.TaskBlueprint;

namespace ddge::scheduler {

/**
 * Requires resource of `Timer_T`
 **/
export template <typename Timer_T, convertible_to_TaskBlueprint_c<void> TaskBlueprint_T>
    requires time::is_FixedTimer_c<Timer_T> || time::is_VariableTimer_c<Timer_T>
[[nodiscard]]
constexpr auto at_fixed_rate(TaskBlueprint_T&& task_blueprint)
    -> TaskBlueprint<void, Cardinality::eSingle>
{
    using TimerResource = accessors::resources::Resource<Timer_T>;

    return repeat(
        std::move(task_blueprint),   //
        as_task([](const TimerResource timer) { return timer->delta_ticks(); })
    );
}

}   // namespace ddge::scheduler
