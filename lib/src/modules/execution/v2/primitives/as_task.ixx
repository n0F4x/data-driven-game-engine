module;

#include <tuple>
#include <utility>

export module ddge.modules.execution.v2.primitives.as_task;

import ddge.modules.execution.Nexus;
import ddge.modules.execution.provide_accessors_for;
import ddge.modules.execution.raw_task_c;
import ddge.modules.execution.scheduler.WorkContinuation;
import ddge.modules.execution.v2.ExecPolicy;
import ddge.modules.execution.v2.Task;
import ddge.modules.execution.v2.TaskBuilder;
import ddge.modules.execution.v2.TaskFinishedCallback;
import ddge.modules.execution.v2.TaskHubBuilder;
import ddge.modules.execution.v2.TaskHubProxy;

import ddge.utility.meta.type_traits.functional.result_of;

namespace ddge::exec::v2 {

export template <ExecPolicy execution_policy_T = ExecPolicy::eDefault, raw_task_c F>
[[nodiscard]]
auto as_task(F&& func) -> TaskBuilder<util::meta::result_of_t<F>>
{
    return TaskBuilder<util::meta::result_of_t<F>>{
        [x_func = std::forward<F>(func)](
            Nexus&                                             nexus,
            TaskHubBuilder&                                    task_hub_builder,
            TaskFinishedCallback<util::meta::result_of_t<F>>&& callback
        ) mutable -> Task {
            return Task{
                task_hub_builder.emplace(
                    [body            = std::move(x_func),
                     accessors_tuple = provide_accessors_for<F>(nexus),
                     x_callback      = std::move(callback)](   //
                        TaskHubProxy& task_hub_proxy
                    ) mutable -> WorkContinuation         //
                    {
                        if constexpr (std::is_void_v<util::meta::result_of_t<F>>) {
                            std::apply(body, accessors_tuple);
                            x_callback(task_hub_proxy);
                        }
                        else {
                            x_callback(task_hub_proxy, std::apply(body, accessors_tuple));
                        }
                        return WorkContinuation::eDontCare;
                    },
                    execution_policy_T
                )   //
            };
        }
    };
}

}   // namespace ddge::exec::v2
