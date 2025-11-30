module;

#include <functional>

export module ddge.modules.scheduler.TaskHubProxy;

import ddge.modules.scheduler.TaskHub;
import ddge.modules.scheduler.TaskIndex;

namespace ddge::scheduler {

export class TaskHubProxy {
public:
    explicit TaskHubProxy(TaskHub& task_hub);

    auto schedule(TaskIndex task_index) const -> void;

private:
    std::reference_wrapper<TaskHub> m_task_hub_ref;
};

}   // namespace ddge::scheduler
