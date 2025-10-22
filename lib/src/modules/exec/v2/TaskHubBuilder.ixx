module;

#include <memory>
#include <vector>

#include <function2/function2.hpp>

export module ddge.modules.exec.v2.TaskHubBuilder;

import ddge.modules.exec.scheduler.Work;
import ddge.modules.exec.v2.ExecPolicy;
import ddge.modules.exec.v2.TaskHub;
import ddge.modules.exec.v2.TaskHubProxy;
import ddge.modules.exec.v2.TaskIndex;

namespace ddge::exec::v2 {

export class TaskHubBuilder {
public:
    auto emplace(
        fu2::unique_function<void(const TaskHubProxy&)>&& task,
        ExecPolicy execution_policy = ExecPolicy::eDefault
    ) -> TaskIndex;

    auto build() && -> std::unique_ptr<TaskHub>;

private:
    using TaskFactory = fu2::unique_function<void(const TaskHubProxy&)>;

    std::vector<TaskFactory> m_generic_work_factories;
    std::vector<TaskFactory> m_main_only_work_factories;
};

}   // namespace ddge::exec::v2
