module;

#include <functional>

export module ddge.modules.execution.v2.TaskHubProxy;

import ddge.modules.execution.scheduler.WorkIndex;
import ddge.modules.execution.v2.TaskHub;

namespace ddge::exec::v2 {

export class TaskHubProxy {
public:
    explicit TaskHubProxy(TaskHub& task_hub);

    auto schedule(WorkIndex work_index) const -> void;

private:
    std::reference_wrapper<TaskHub> m_task_hub_ref;
};

}   // namespace ddge::exec::v2
