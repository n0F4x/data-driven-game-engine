module;

#include <tuple>
#include <utility>

export module core.scheduler.wrap_as_builder;

import core.scheduler.Nexus;
import core.scheduler.provide_accessors_for;
import core.scheduler.Task;
import core.scheduler.TaskBuilder;
import core.scheduler.SchedulerBuilder;

import utility.meta.concepts.functional.unambiguously_invocable;
import utility.meta.type_traits.functional.result_of;

namespace core::scheduler {

export template <util::meta::unambiguously_invocable_c F>
[[nodiscard]]
auto wrap_as_builder(F&& func) -> TaskBuilder<util::meta::result_of_t<F>>;

export template <typename TaskBuilder_T>
    requires specialization_of_TaskBuilder_c<std::remove_cvref_t<TaskBuilder_T>>
[[nodiscard]]
auto wrap_as_builder(TaskBuilder_T&& task_builder) -> std::remove_cvref_t<TaskBuilder_T>;

export template <typename SchedulerBuilder_T>
    requires std::same_as<std::remove_cvref_t<SchedulerBuilder_T>, SchedulerBuilder>
[[nodiscard]]
auto wrap_as_builder(SchedulerBuilder_T&& scheduler_builder) -> TaskBuilder<void>;

}   // namespace core::scheduler

template <util::meta::unambiguously_invocable_c F>
auto core::scheduler::wrap_as_builder(F&& func)
    -> TaskBuilder<util::meta::result_of_t<F>>
{
    return TaskBuilder<util::meta::result_of_t<F>>{
        [wrapped_func = std::forward<F>(func
         )](Nexus& nexus) -> Task<util::meta::result_of_t<F>> {
            return Task<util::meta::result_of_t<F>>{
                [wrapped_func, accessors_tuple = provide_accessors_for<F>(nexus)]
                -> util::meta::result_of_t<F> {
                    return std::apply(wrapped_func, accessors_tuple);
                }
            };
        }
    };
}

template <typename TaskBuilder_T>
    requires core::scheduler::
        specialization_of_TaskBuilder_c<std::remove_cvref_t<TaskBuilder_T>>
    auto core::scheduler::wrap_as_builder(TaskBuilder_T&& task_builder)
        -> std::remove_cvref_t<TaskBuilder_T>
{
    return std::forward<TaskBuilder_T>(task_builder);
}

template <typename SchedulerBuilder_T>
    requires std::same_as<
        std::remove_cvref_t<SchedulerBuilder_T>,
        core::scheduler::SchedulerBuilder>
auto core::scheduler::wrap_as_builder(SchedulerBuilder_T&& scheduler_builder)
    -> TaskBuilder<void>
{
    return std::forward<SchedulerBuilder_T>(scheduler_builder);
}
