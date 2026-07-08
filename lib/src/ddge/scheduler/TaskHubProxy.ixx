module;

#include <functional>

export module ddge.scheduler.TaskHubProxy;

import ddge.scheduler.TaskHub;
import ddge.scheduler.TaskIndex;

namespace ddge::scheduler {

export class TaskHubProxy {
public:
    explicit TaskHubProxy(TaskHub& task_hub);

    auto schedule(TaskIndex task_index) const -> void;

private:
    std::reference_wrapper<TaskHub> m_task_hub_ref;
};

}   // namespace ddge::scheduler
