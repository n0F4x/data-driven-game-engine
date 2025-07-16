module;

#include <utility>

export module core.scheduler.at_fixed_rate;

import core.time.FixedTimer;

import core.scheduler.repeat;
import core.scheduler.concepts.converts_to_task_builder_c;

import extensions.scheduler.accessors.resources;

namespace core::scheduler {

/**
 * Requires resource of `core::time::FixedTimer`
 **/
export template <
    core::time::specialization_of_FixedTimer_c FixedTimer_T,
    converts_to_task_builder_c                 TaskBuilder_T>
[[nodiscard]]
constexpr auto at_fixed_rate(TaskBuilder_T&& task_builder);

}   // namespace core::scheduler

template <
    core::time::specialization_of_FixedTimer_c  FixedTimer_T,
    core::scheduler::converts_to_task_builder_c TaskBuilder_T>
constexpr auto core::scheduler::at_fixed_rate(TaskBuilder_T&& task_builder)
{
    using FixedTimerResource =
        extensions::scheduler::accessors::resources::Resource<FixedTimer_T>;

    return core::scheduler::repeat(
        std::forward<TaskBuilder_T>(task_builder),
        [](const FixedTimerResource timer) { return timer->delta_ticks(); }
    );
}
