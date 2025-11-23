module;

#include <functional>
#include <vector>

#include <function2/function2.hpp>

export module ddge.modules.exec.v2.TaskHubBuilder;

import ddge.modules.exec.Nexus;
import ddge.modules.exec.provide_accessors_for;
import ddge.modules.exec.raw_task_c;
import ddge.modules.exec.scheduler.Work;
import ddge.modules.exec.v2.ExecPolicy;
import ddge.modules.exec.v2.TaskFinishedCallback;
import ddge.modules.exec.v2.TaskHub;
import ddge.modules.exec.v2.TaskHubProxy;
import ddge.modules.exec.v2.TaskIndex;

import ddge.utility.meta.type_traits.functional.result_of;

namespace ddge::exec::v2 {

export class TaskHubBuilder {
public:
    explicit TaskHubBuilder(Nexus& nexus);

    template <raw_task_c F>
    auto emplace(
        F&&                                                task,
        TaskFinishedCallback<util::meta::result_of_t<F>>&& callback,
        ExecPolicy                                         execution_policy
    ) -> TaskIndex;

    auto build() && -> std::unique_ptr<TaskHub>;

private:
    using TaskFactory = fu2::unique_function<void(const TaskHubProxy&)>;

    std::reference_wrapper<Nexus> m_nexus;
    std::vector<TaskFactory>      m_generic_work_factories;
    std::vector<TaskFactory>      m_main_only_work_factories;

    auto emplace(
        fu2::unique_function<void(const TaskHubProxy&)>&& task,
        ExecPolicy                                        execution_policy
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
        [body            = std::move(task),
         accessors_tuple = provide_accessors_for<F>(m_nexus),
         x_callback      = std::move(callback)]                     //
        (const TaskHubProxy& task_hub_proxy) mutable -> void   //
        {
            if constexpr (std::is_void_v<Result>) {
                std::apply(body, accessors_tuple);
                x_callback(task_hub_proxy);
            }
            else {
                x_callback(task_hub_proxy, std::apply(body, accessors_tuple));
            }
        },
        execution_policy
    );
}
