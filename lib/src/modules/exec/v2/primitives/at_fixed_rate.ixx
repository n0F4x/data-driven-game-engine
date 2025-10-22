module;

#include <utility>

export module ddge.modules.exec.v2.primitives.at_fixed_rate;

import ddge.modules.time.FixedTimer;
import ddge.modules.time.VariableTimer;

import ddge.modules.exec.accessors.resources.Resource;
import ddge.modules.exec.providers.ResourceProvider;
import ddge.modules.exec.v2.convertible_to_TaskBlueprint_c;
import ddge.modules.exec.v2.Cardinality;
import ddge.modules.exec.v2.primitives.as_task;
import ddge.modules.exec.v2.primitives.repeat;
import ddge.modules.exec.v2.TaskBlueprint;

namespace ddge::exec::v2 {

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

}   // namespace ddge::exec::v2
