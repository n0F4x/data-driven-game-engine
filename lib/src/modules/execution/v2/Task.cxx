module;

#include <variant>

#include <function2/function2.hpp>

module ddge.modules.execution.v2.Task;

import ddge.utility.Overloaded;

ddge::exec::v2::Task::Task(WorkIndex work_index) : m_task{ work_index } {}

auto ddge::exec::v2::Task::schedule(const TaskHubProxy& task_hub_proxy) const -> void
{
    std::visit(
        util::Overloaded{
            [&task_hub_proxy](const WorkIndex work_index) {
                task_hub_proxy.schedule(work_index);
            },
            [&task_hub_proxy](const fu2::function<void(const TaskHubProxy&) const>& work) {
                work(task_hub_proxy);
            }   //
        },
        m_task
    );
}
