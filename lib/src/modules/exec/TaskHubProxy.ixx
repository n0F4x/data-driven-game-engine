module;

#include <functional>

export module ddge.modules.exec.TaskHubProxy;

import ddge.modules.exec.TaskHub;
import ddge.modules.exec.TaskIndex;

namespace ddge::exec {

export class TaskHubProxy {
public:
    explicit TaskHubProxy(TaskHub& task_hub);

    auto schedule(TaskIndex task_index) const -> void;

private:
    std::reference_wrapper<TaskHub> m_task_hub_ref;
};

}   // namespace ddge::exec
