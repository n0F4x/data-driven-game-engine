module;

#include <utility>

export module extensions.scheduler.primitives.at_fixed_rate;

import core.time.FixedTimer;

import core.scheduler.converts_to_task_builder_c;
import core.scheduler.TaskBuilder;

import extensions.scheduler.accessors.resources;
import extensions.scheduler.primitives.repeat;

namespace extensions::scheduler {

/**
 * Requires resource of `FixedTimer_T`
 **/
export template <
    core::time::specialization_of_FixedTimer_c  FixedTimer_T,
    core::scheduler::converts_to_task_builder_c TaskBuilder_T>
[[nodiscard]]
constexpr auto at_fixed_rate(TaskBuilder_T&& task_builder)
    -> core::scheduler::TaskBuilder<void>;

}   // namespace extensions::scheduler

template <
    core::time::specialization_of_FixedTimer_c  FixedTimer_T,
    core::scheduler::converts_to_task_builder_c TaskBuilder_T>
constexpr auto extensions::scheduler::at_fixed_rate(TaskBuilder_T&& task_builder)
    -> core::scheduler::TaskBuilder<void>
{
    using FixedTimerResource =
        extensions::scheduler::accessors::resources::Resource<FixedTimer_T>;

    return extensions::scheduler::repeat(
        std::forward<TaskBuilder_T>(task_builder),
        [](const FixedTimerResource timer) { return timer->delta_ticks(); }
    );
}
