module;

#include <utility>

export module core.scheduler.all_of;

import core.scheduler.as_task_builder;
import core.scheduler.concepts.converts_to_predicate_task_builder_c;
import core.scheduler.task_builders.AllOfTaskBuilder;

namespace core::scheduler {

export template <converts_to_predicate_task_builder_c... PredicateTaskBuilders_T>
[[nodiscard]]
constexpr auto all_of(PredicateTaskBuilders_T&&... predicate_task_builders)
    -> AllOfTaskBuilder<as_task_builder_t<PredicateTaskBuilders_T>...>;

}   // namespace core::scheduler

template <core::scheduler::converts_to_predicate_task_builder_c... PredicateTaskBuilders_T>
constexpr auto core::scheduler::all_of(
    PredicateTaskBuilders_T&&... predicate_task_builders
) -> AllOfTaskBuilder<as_task_builder_t<PredicateTaskBuilders_T>...>
{
    return AllOfTaskBuilder<as_task_builder_t<PredicateTaskBuilders_T>...>{
        std::forward<PredicateTaskBuilders_T>(predicate_task_builders)...
    };
}
