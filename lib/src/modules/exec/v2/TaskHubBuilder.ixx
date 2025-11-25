module;

#include <functional>
#include <vector>

#include "utility/contracts_macros.hpp"

export module ddge.modules.exec.v2.TaskHubBuilder;

import ddge.modules.exec.Nexus;
import ddge.modules.exec.provide_accessors_for;
import ddge.modules.exec.raw_task_c;
import ddge.modules.exec.scheduler.Work;
import ddge.modules.exec.v2.EmbeddedTaskBody;
import ddge.modules.exec.v2.EmbeddedTaskFactory;
import ddge.modules.exec.v2.ErasedTaskFactory;
import ddge.modules.exec.v2.ExecPolicy;
import ddge.modules.exec.v2.TaskContinuation;
import ddge.modules.exec.v2.TaskContinuationFactory;
import ddge.modules.exec.v2.TaskFinishedCallback;
import ddge.modules.exec.v2.TaskHub;
import ddge.modules.exec.v2.TaskHubProxy;
import ddge.modules.exec.v2.TaskIndex;

import ddge.utility.contracts;
import ddge.utility.meta.type_traits.functional.result_of;

namespace ddge::exec::v2 {

export class TaskHubBuilder {
public:
    explicit TaskHubBuilder(Nexus& nexus);

    template <raw_task_c F>
    [[nodiscard]]
    auto emplace(
        F&&                                                task,
        TaskFinishedCallback<util::meta::result_of_t<F>>&& callback,
        ExecPolicy                                         execution_policy
    ) -> TaskIndex;

    auto build() && -> std::unique_ptr<TaskHub>;

private:
    std::reference_wrapper<Nexus>  m_nexus;
    std::vector<ErasedTaskFactory> m_generic_task_factories;
    std::vector<ErasedTaskFactory> m_main_only_task_factories;

    template <typename Result_T>
    [[nodiscard]]
    auto emplace(
        EmbeddedTaskBody<Result_T>&&        task_body,
        TaskContinuationFactory<Result_T>&& task_continuation_factory,
        ExecPolicy                          execution_policy
    ) -> TaskIndex;
};

}   // namespace ddge::exec::v2

template <ddge::exec::raw_task_c F>
auto ddge::exec::v2::TaskHubBuilder::emplace(
    F&&                                                task,
    TaskFinishedCallback<util::meta::result_of_t<F>>&& callback,
    ExecPolicy                                         execution_policy
) -> TaskIndex
{
    using Result = util::meta::result_of_t<F>;

    return emplace(
        EmbeddedTaskBody<Result>{
            [body            = std::move(task),
             accessors_tuple = provide_accessors_for<F>(m_nexus)] mutable -> Result {
                return std::apply(body, accessors_tuple);
            } },
        TaskContinuationFactory<Result>{
            [x_callback = std::move(callback)](const TaskHubProxy task_hub_proxy) mutable
                -> TaskContinuation<Result>   //
            {
                if constexpr (std::is_void_v<Result>) {
                    return TaskContinuation<Result>{
                        [y_callback       = std::move(x_callback),
                         x_task_hub_proxy = task_hub_proxy] mutable -> void {
                            y_callback(x_task_hub_proxy);
                        }
                    };
                }
                else {
                    return TaskContinuation<Result>{
                        [y_callback       = std::move(x_callback),
                         x_task_hub_proxy = task_hub_proxy]   //
                        (Result&& result) mutable -> void {
                            y_callback(x_task_hub_proxy, std::move(result));
                        }   //
                    };
                }
            }   //
        },
        execution_policy
    );
}

template <typename Result_T>
auto ddge::exec::v2::TaskHubBuilder::emplace(
    EmbeddedTaskBody<Result_T>&&        task_body,
    TaskContinuationFactory<Result_T>&& task_continuation_factory,
    const ExecPolicy                    execution_policy
) -> TaskIndex
{
    std::vector<ErasedTaskFactory>& task_factories{
        [this, execution_policy] -> std::vector<ErasedTaskFactory>& {
            switch (execution_policy) {
                case ExecPolicy::eDefault:     return m_generic_task_factories;
                case ExecPolicy::eForceOnMain: return m_main_only_task_factories;
            }
        }()   //
    };
    const TaskIndex::Underlying index_mask{
        TaskHub::IndexTagMasks::get(execution_policy)
    };

    PRECOND(
        task_factories.size() < (std::numeric_limits<TaskIndex::Underlying>::max() >> 2)
    );

    task_factories.push_back(
        ErasedTaskFactory{
            EmbeddedTaskFactory<Result_T>{ std::move(task_body) }
                .set_continuation_factory(std::move(task_continuation_factory))   //
        }
    );

    return TaskIndex{ (task_factories.size() - 1) | index_mask };
}
