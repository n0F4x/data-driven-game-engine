module;

#include <utility>

export module ddge.modules.exec.v2.TaskBuilder;

import ddge.modules.exec.v2.TaskFinishedCallback;
import ddge.modules.exec.v2.TaskHubBuilder;
import ddge.modules.exec.v2.TypedTaskIndex;

import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::exec::v2 {

export template <typename Result_T>
class TaskBuilder {
    using BuildFunc = util::AnyMoveOnlyFunction<
        TypedTaskIndex<Result_T>(TaskHubBuilder&, TaskFinishedCallback<Result_T>&&) &&>;

public:
    template <typename F>
    explicit TaskBuilder(F&& build) : m_build{ std::forward<F>(build) }
    {}

    [[nodiscard]]
    auto build(
        TaskHubBuilder&                  task_hub_builder,
        TaskFinishedCallback<Result_T>&& callback
    ) && -> TypedTaskIndex<Result_T>
    {
        return std::move(m_build)(task_hub_builder, std::move(callback));
    }

private:
    BuildFunc m_build;
};

}   // namespace ddge::exec::v2
