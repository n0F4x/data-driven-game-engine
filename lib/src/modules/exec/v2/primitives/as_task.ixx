module;

#include <utility>

export module ddge.modules.exec.v2.primitives.as_task;

import ddge.modules.exec.raw_task_c;
import ddge.modules.exec.v2.Cardinality;
import ddge.modules.exec.v2.ExecPolicy;
import ddge.modules.exec.v2.TaskBlueprint;
import ddge.modules.exec.v2.TaskBuilder;
import ddge.modules.exec.v2.TaskBundle;
import ddge.modules.exec.v2.TaskFinishedCallback;
import ddge.modules.exec.v2.TaskHubBuilder;

import ddge.utility.meta.type_traits.functional.result_of;

namespace ddge::exec::v2 {

export template <ExecPolicy execution_policy_T = ExecPolicy::eDefault, raw_task_c F>
[[nodiscard]]
auto as_task(F&& func) -> TaskBlueprint<util::meta::result_of_t<F>, Cardinality::eSingle>
{
    using Result = util::meta::result_of_t<F>;

    return TaskBlueprint<Result, Cardinality::eSingle>{
        [x_func = std::forward<F>(func)]() mutable -> TaskBuilder<Result> {
            return TaskBuilder<Result>{
                [y_func = std::move(x_func)](
                    TaskHubBuilder&                                    task_hub_builder,
                    TaskFinishedCallback<util::meta::result_of_t<F>>&& callback
                ) mutable -> TaskBundle {
                    return TaskBundle{
                        task_hub_builder.emplace(
                            std::move(y_func),
                            std::move(callback),
                            execution_policy_T
                        )   //
                    };
                }   //
            };
        }   //
    };
}

}   // namespace ddge::exec::v2
