module;

#include <function2/function2.hpp>

export module ddge.modules.execution.v2.TaskBuilder;

import ddge.modules.execution.Nexus;
import ddge.modules.execution.v2.TaskIndex;
import ddge.modules.execution.v2.TaskFinishedCallback;
import ddge.modules.execution.v2.TaskHubBuilder;

namespace ddge::exec::v2 {

export template <typename Result_T>
class TaskBuilder {
public:
    explicit TaskBuilder(
        fu2::unique_function<
            TaskIndex(Nexus&, TaskHubBuilder&, TaskFinishedCallback<Result_T>&&)>&& build
    );

    [[nodiscard]]
    auto build(
        Nexus&                           nexus,
        TaskHubBuilder&                  task_hub_builder,
        TaskFinishedCallback<Result_T>&& callback = nullptr
    ) && -> TaskIndex;

private:
    fu2::unique_function<
        TaskIndex(Nexus&, TaskHubBuilder&, TaskFinishedCallback<Result_T>&&)>
        m_build;
};

}   // namespace ddge::exec::v2

template <typename Result_T>
ddge::exec::v2::TaskBuilder<Result_T>::TaskBuilder(
    fu2::unique_function<
        TaskIndex(Nexus&, TaskHubBuilder&, TaskFinishedCallback<Result_T>&&)>&& build
)
    : m_build{ std::move(build) }
{}

template <typename Result_T>
auto ddge::exec::v2::TaskBuilder<Result_T>::build(
    Nexus&                           nexus,
    TaskHubBuilder&                  task_hub_builder,
    TaskFinishedCallback<Result_T>&& callback
) && -> TaskIndex
{
    return std::move(*this).m_build(nexus, task_hub_builder, std::move(callback));
}
