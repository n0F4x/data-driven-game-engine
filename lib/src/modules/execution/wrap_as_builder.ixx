module;

#include <concepts>
#include <tuple>
#include <type_traits>
#include <utility>

export module ddge.modules.execution.wrap_as_builder;

import ddge.modules.execution.Nexus;
import ddge.modules.execution.provide_accessors_for;
import ddge.modules.execution.raw_task_c;
import ddge.modules.execution.Task;
import ddge.modules.execution.TaskBuilder;
import ddge.modules.execution.SchedulerBuilder;

import ddge.utility.meta.concepts.functional.unambiguously_invocable;
import ddge.utility.meta.concepts.type_list.type_list_all_of;
import ddge.utility.meta.type_traits.functional.arguments_of;
import ddge.utility.meta.type_traits.functional.result_of;

namespace ddge::exec {

export template <raw_task_c F>
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

}   // namespace ddge::exec

template <ddge::exec::raw_task_c F>
auto ddge::exec::wrap_as_builder(F&& func) -> TaskBuilder<util::meta::result_of_t<F>>
{
    return TaskBuilder<util::meta::result_of_t<F>>{
        [wrapped_func = std::forward<F>(func
         )](Nexus& nexus) -> Task<util::meta::result_of_t<F>> {
            return Task<util::meta::result_of_t<F>>{
                [wrapped_func, accessors_tuple = provide_accessors_for<F>(nexus)] mutable
                    -> util::meta::result_of_t<F> {
                    return std::apply(wrapped_func, accessors_tuple);
                }
            };
        }
    };
}

template <typename TaskBuilder_T>
    requires ddge::exec::
        specialization_of_TaskBuilder_c<std::remove_cvref_t<TaskBuilder_T>>
    auto ddge::exec::wrap_as_builder(TaskBuilder_T&& task_builder)
        -> std::remove_cvref_t<TaskBuilder_T>
{
    return std::forward<TaskBuilder_T>(task_builder);
}

template <typename SchedulerBuilder_T>
    requires std::
        same_as<std::remove_cvref_t<SchedulerBuilder_T>, ddge::exec::SchedulerBuilder>
    auto ddge::exec::wrap_as_builder(SchedulerBuilder_T&& scheduler_builder)
        -> TaskBuilder<void>
{
    return std::forward<SchedulerBuilder_T>(scheduler_builder);
}
