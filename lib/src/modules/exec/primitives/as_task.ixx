module;

#include <utility>

export module ddge.modules.exec.primitives.as_task;

import ddge.modules.exec.raw_task_c;
import ddge.modules.exec.Cardinality;
import ddge.modules.exec.ExecPolicy;
import ddge.modules.exec.TaskBlueprint;
import ddge.modules.exec.TaskBuilder;
import ddge.modules.exec.TaskHubBuilder;
import ddge.modules.exec.TypedTaskIndex;

import ddge.utility.meta.type_traits.functional.result_of;

namespace ddge::exec {

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

}   // namespace ddge::exec
