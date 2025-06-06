module;

#include <utility>

export module core.scheduler.run_if;

import core.scheduler.as_task_builder;
import core.scheduler.concepts.converts_to_task_builder_c;
import core.scheduler.concepts.converts_to_predicate_task_builder_c;
import core.scheduler.task_builders.ConditionalTaskBuilder;

namespace core::scheduler {

export template <
    converts_to_task_builder_c           MainTaskBuilder_T,
    converts_to_predicate_task_builder_c PredicateTaskBuilder_T>
[[nodiscard]]
constexpr auto run_if(
    MainTaskBuilder_T&&      main_task_builder,
    PredicateTaskBuilder_T&& predicate_task_builder
)
    -> ConditionalTaskBuilder<
        as_task_builder_t<MainTaskBuilder_T>,
        as_task_builder_t<PredicateTaskBuilder_T>>;

}   // namespace core::scheduler

template <
    core::scheduler::converts_to_task_builder_c           MainTaskBuilder_T,
    core::scheduler::converts_to_predicate_task_builder_c PredicateTaskBuilder_T>
constexpr auto core::scheduler::run_if(
    MainTaskBuilder_T&&      main_task_builder,
    PredicateTaskBuilder_T&& predicate_task_builder
)
    -> ConditionalTaskBuilder<
        as_task_builder_t<MainTaskBuilder_T>,
        as_task_builder_t<PredicateTaskBuilder_T>>
{
    return ConditionalTaskBuilder<
        as_task_builder_t<MainTaskBuilder_T>,
        as_task_builder_t<PredicateTaskBuilder_T>>{
        std::forward<MainTaskBuilder_T>(main_task_builder),
        std::forward<PredicateTaskBuilder_T>(predicate_task_builder)
    };
}
