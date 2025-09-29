module;

#include <variant>

#include <function2/function2.hpp>

export module ddge.modules.execution.v2.Task;

import ddge.modules.execution.scheduler.WorkIndex;
import ddge.modules.execution.v2.TaskHubProxy;

namespace ddge::exec::v2 {

export class Task {
public:
    explicit Task(WorkIndex work_index);

    auto schedule(const TaskHubProxy& task_hub_proxy) const -> void;

private:
    std::variant<WorkIndex, fu2::function<void(const TaskHubProxy&) const>> m_task;
};

}   // namespace ddge::exec::v2
