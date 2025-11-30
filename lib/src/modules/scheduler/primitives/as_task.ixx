module;

#include <utility>

export module ddge.modules.scheduler.primitives.as_task;

import ddge.modules.scheduler.raw_task_c;
import ddge.modules.scheduler.Cardinality;
import ddge.modules.scheduler.ExecPolicy;
import ddge.modules.scheduler.TaskBlueprint;
import ddge.modules.scheduler.TaskBuilder;
import ddge.modules.scheduler.TaskHubBuilder;
import ddge.modules.scheduler.TypedTaskIndex;

import ddge.utility.meta.type_traits.functional.result_of;

namespace ddge::scheduler {

export template <ExecPolicy execution_policy_T = ExecPolicy::eDefault, raw_task_c F>
[[nodiscard]]
auto as_task(F&& func) -> TaskBlueprint<util::meta::result_of_t<F>, Cardinality::eSingle>
{
    using Result = util::meta::result_of_t<F>;

    return TaskBlueprint<Result, Cardinality::eSingle>{
        [x_func = std::forward<F>(func)]() mutable -> TaskBuilder<Result> {
            return TaskBuilder<Result>{
                [y_func = std::move(x_func)](
                    TaskHubBuilder& task_hub_builder
                ) mutable -> TypedTaskIndex<Result> {
                    return task_hub_builder.emplace_embedded_task(
                        std::move(y_func), execution_policy_T
                    );
                }   //
            };
        }   //
    };
}

}   // namespace ddge::scheduler
