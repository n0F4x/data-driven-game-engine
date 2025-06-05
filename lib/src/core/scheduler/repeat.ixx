module;

#include <type_traits>

export module core.scheduler.repeat;

import core.scheduler.as_task_builder;
import core.scheduler.concepts.converts_to_task_builder_c;
import core.scheduler.concepts.converts_to_predicate_task_builder_c;
import core.scheduler.task_builders.RepeatedTaskBuilder;

namespace core::scheduler {

export template <
    converts_to_task_builder_c           MainTaskBuilder_T,
    converts_to_predicate_task_builder_c RepetitionSpecifierTaskBuilder_T>
[[nodiscard]]
constexpr auto repeat(
    MainTaskBuilder_T&&                main_task_builder,
    RepetitionSpecifierTaskBuilder_T&& repetition_specifier_task_builder
)
    -> RepeatedTaskBuilder<
        as_task_builder_t<MainTaskBuilder_T>,
        as_task_builder_t<RepetitionSpecifierTaskBuilder_T>>;

}   // namespace core::scheduler

template <
    core::scheduler::converts_to_task_builder_c MainTaskBuilder_T,
    core::scheduler::converts_to_predicate_task_builder_c RepetitionSpecifierTaskBuilder_T>
constexpr auto core::scheduler::repeat(
    MainTaskBuilder_T&&                main_task_builder,
    RepetitionSpecifierTaskBuilder_T&& repetition_specifier_task_builder
)
    -> RepeatedTaskBuilder<
        as_task_builder_t<MainTaskBuilder_T>,
        as_task_builder_t<RepetitionSpecifierTaskBuilder_T>>
{
    return RepeatedTaskBuilder<
        as_task_builder_t<MainTaskBuilder_T>,
        as_task_builder_t<RepetitionSpecifierTaskBuilder_T>>{
        std::forward<MainTaskBuilder_T>(main_task_builder),
        std::forward<RepetitionSpecifierTaskBuilder_T>(repetition_specifier_task_builder)
    };
}
