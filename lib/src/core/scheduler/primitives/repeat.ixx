module;

#include <concepts>
#include <ranges>
#include <utility>

export module core.scheduler.primitives.repeat;

import core.scheduler.Nexus;
import core.scheduler.as_task_builder_t;
import core.scheduler.converts_to_task_builder_c;
import core.scheduler.Task;
import core.scheduler.TaskBuilder;
import core.scheduler.wrap_as_builder;

namespace core::scheduler {

inline namespace primitives {

export template <
    core::scheduler::converts_to_task_builder_c TaskBuilder_T,
    core::scheduler::converts_to_task_builder_c RepetitionSpecifierTaskBuilder_T>
    requires std::same_as<
                 typename core::scheduler::as_task_builder_t<TaskBuilder_T>::Result,
                 void>
          && std::integral<typename core::scheduler::
                               as_task_builder_t<RepetitionSpecifierTaskBuilder_T>::Result>
[[nodiscard]]
auto repeat(
    TaskBuilder_T&&                    task_builder,
    RepetitionSpecifierTaskBuilder_T&& repetition_specifier_task_builder
) -> core::scheduler::TaskBuilder<void>;

}   // namespace primitives

}   // namespace core::scheduler

template <
    core::scheduler::converts_to_task_builder_c TaskBuilder_T,
    core::scheduler::converts_to_task_builder_c RepetitionSpecifierTaskBuilder_T>
    requires std::same_as<
                 typename core::scheduler::as_task_builder_t<TaskBuilder_T>::Result,
                 void>
          && std::integral<typename core::scheduler::
                               as_task_builder_t<RepetitionSpecifierTaskBuilder_T>::Result>
auto core::scheduler::repeat(
    TaskBuilder_T&&                    task_builder,
    RepetitionSpecifierTaskBuilder_T&& repetition_specifier_task_builder
) -> core::scheduler::TaskBuilder<void>
{
    return core::scheduler::TaskBuilder<void>{
        [wrapped_task_builder =
             core::scheduler::wrap_as_builder(std::forward<TaskBuilder_T>(task_builder)),
         wrapped_repetition_specifier_task_builder = core::scheduler::wrap_as_builder(
             std::forward<RepetitionSpecifierTaskBuilder_T>(
                 repetition_specifier_task_builder
             )
         )](core::scheduler::Nexus& nexus) -> core::scheduler::Task<void> {
            return [task                      = wrapped_task_builder.build(nexus),
                    repetition_specifier_task = wrapped_repetition_specifier_task_builder
                                                    .build(nexus)] mutable -> void {
                for (const auto _ : std::views::repeat(
                         std::ignore, std::invoke(repetition_specifier_task)
                     ))
                {
                    task();
                }
            };
        }
    };
}
