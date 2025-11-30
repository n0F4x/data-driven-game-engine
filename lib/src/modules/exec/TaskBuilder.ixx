module;

#include <utility>

export module ddge.modules.exec.TaskBuilder;

import ddge.modules.exec.TaskHubBuilder;
import ddge.modules.exec.TypedTaskIndex;

import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::exec {

export template <typename Result_T>
class TaskBuilder {
    using BuildFunc =
        util::AnyMoveOnlyFunction<TypedTaskIndex<Result_T>(TaskHubBuilder&) &&>;

public:
    template <typename F>
    explicit TaskBuilder(F&& build) : m_build{ std::forward<F>(build) }
    {}

    [[nodiscard]]
    auto build(TaskHubBuilder& task_hub_builder) && -> TypedTaskIndex<Result_T>
    {
        return std::move(m_build)(task_hub_builder);
    }

private:
    BuildFunc m_build;
};

}   // namespace ddge::exec
