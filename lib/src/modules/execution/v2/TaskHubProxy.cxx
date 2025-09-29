module ddge.modules.execution.v2.TaskHubProxy;

ddge::exec::v2::TaskHubProxy::TaskHubProxy(TaskHub& task_hub) : m_task_hub_ref{ task_hub }
{}

auto ddge::exec::v2::TaskHubProxy::schedule(const WorkIndex work_index) const -> void
{
    m_task_hub_ref.get().schedule(work_index);
}
