module;

#include <functional>

export module ddge.modules.exec.v2.TaskHubProxy;

import ddge.modules.exec.v2.TaskHub;
import ddge.modules.exec.v2.TaskIndex;

namespace ddge::exec::v2 {

export class TaskHubProxy {
public:
    explicit TaskHubProxy(TaskHub& task_hub);

    auto schedule(TaskIndex task_index) const -> void;

private:
    std::reference_wrapper<TaskHub> m_task_hub_ref;
};

}   // namespace ddge::exec::v2
