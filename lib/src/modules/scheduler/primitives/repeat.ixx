module;

#include <concepts>
#include <ranges>
#include <utility>

export module modules.scheduler.primitives.repeat;

import modules.scheduler.Nexus;
import modules.scheduler.as_task_builder_t;
import modules.scheduler.converts_to_task_builder_c;
import modules.scheduler.Task;
import modules.scheduler.TaskBuilder;
import modules.scheduler.wrap_as_builder;

namespace modules::scheduler {

inline namespace primitives {

export template <
    converts_to_task_builder_c TaskBuilder_T,
    converts_to_task_builder_c RepetitionSpecifierTaskBuilder_T>
    requires std::same_as<
                 typename as_task_builder_t<TaskBuilder_T>::Result,
                 void>
          && std::integral<typename
                               as_task_builder_t<RepetitionSpecifierTaskBuilder_T>::Result>
[[nodiscard]]
auto repeat(
    TaskBuilder_T&&                    task_builder,
    RepetitionSpecifierTaskBuilder_T&& repetition_specifier_task_builder
) -> TaskBuilder<void>;

}   // namespace primitives

}   // namespace modules::scheduler

template <
    modules::scheduler::converts_to_task_builder_c TaskBuilder_T,
    modules::scheduler::converts_to_task_builder_c RepetitionSpecifierTaskBuilder_T>
    requires std::same_as<
                 typename modules::scheduler::as_task_builder_t<TaskBuilder_T>::Result,
                 void>
          && std::integral<typename modules::scheduler::
                               as_task_builder_t<RepetitionSpecifierTaskBuilder_T>::Result>
auto modules::scheduler::repeat(
    TaskBuilder_T&&                    task_builder,
    RepetitionSpecifierTaskBuilder_T&& repetition_specifier_task_builder
) -> TaskBuilder<void>
{
    return TaskBuilder<void>{
        [wrapped_task_builder =
             wrap_as_builder(std::forward<TaskBuilder_T>(task_builder)),
         wrapped_repetition_specifier_task_builder = wrap_as_builder(
             std::forward<RepetitionSpecifierTaskBuilder_T>(
                 repetition_specifier_task_builder
             )
         )](Nexus& nexus) -> Task<void> {
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
