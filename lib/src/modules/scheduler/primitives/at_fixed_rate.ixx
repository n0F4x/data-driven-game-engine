module;

#include <utility>

export module modules.scheduler.primitives.at_fixed_rate;

import modules.time.FixedTimer;
import modules.time.VariableTimer;

import modules.scheduler.converts_to_task_builder_c;
import modules.scheduler.TaskBuilder;

import modules.scheduler.accessors.resources;
import modules.scheduler.primitives.repeat;

namespace modules::scheduler {

/**
 * Requires resource of `Timer_T`
 **/
export template <typename Timer_T, converts_to_task_builder_c TaskBuilder_T>
    requires time::is_FixedTimer_c<Timer_T>
          || time::is_VariableTimer_c<Timer_T>
[[nodiscard]]
constexpr auto at_fixed_rate(TaskBuilder_T&& task_builder)
    -> TaskBuilder<void>;

}   // namespace modules::scheduler

template <typename Timer_T, modules::scheduler::converts_to_task_builder_c TaskBuilder_T>
    requires modules::time::is_FixedTimer_c<Timer_T>
          || modules::time::is_VariableTimer_c<Timer_T>
constexpr auto modules::scheduler::at_fixed_rate(TaskBuilder_T&& task_builder)
    -> TaskBuilder<void>
{
    using FixedTimerResource =
        accessors::resources::Resource<Timer_T>;

    return repeat(
        std::forward<TaskBuilder_T>(task_builder),
        [](const FixedTimerResource timer) { return timer->delta_ticks(); }
    );
}
