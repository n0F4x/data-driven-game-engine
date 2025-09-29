module;

#include <memory>
#include <vector>

#include <function2/function2.hpp>

export module ddge.modules.execution.v2.TaskHubBuilder;

import ddge.modules.execution.scheduler.Work;
import ddge.modules.execution.scheduler.WorkIndex;
import ddge.modules.execution.v2.ExecPolicy;
import ddge.modules.execution.v2.TaskHub;
import ddge.modules.execution.v2.TaskHubProxy;

namespace ddge::exec::v2 {

export class TaskHubBuilder {
public:
    auto emplace(
        fu2::unique_function<void(TaskHubProxy&)>&& task,
        ExecPolicy execution_policy = ExecPolicy::eDefault
    ) -> WorkIndex;

    auto build() && -> std::unique_ptr<TaskHub>;

private:
    using WorkFactory = fu2::unique_function<void(TaskHubProxy&)>;

    std::vector<WorkFactory> m_generic_work_factories;
    std::vector<WorkFactory> m_main_only_work_factories;
};

}   // namespace ddge::exec::v2
