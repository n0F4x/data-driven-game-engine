module ddge.modules.scheduler.TaskHubProxy;

ddge::scheduler::TaskHubProxy::TaskHubProxy(TaskHub& task_hub)
    : m_task_hub_ref{ task_hub }
{}

auto ddge::scheduler::TaskHubProxy::schedule(const TaskIndex task_index) const -> void
{
    m_task_hub_ref.get().schedule(task_index);
}
