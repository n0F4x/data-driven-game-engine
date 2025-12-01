module;

#include <utility>

export module ddge.modules.scheduler.TaskBuilder;

import ddge.modules.scheduler.TaskHubBuilder;
import ddge.modules.scheduler.TaskResultTrait;
import ddge.modules.scheduler.TypedTaskFactoryHandle;

import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::scheduler {

export template <typename Result_T>
class TaskBuilder {
    using BuildFunc =
        util::AnyMoveOnlyFunction<TypedTaskFactoryHandle<Result_T>(TaskHubBuilder&) &&>;

public:
    template <typename F>
    explicit TaskBuilder(F&& build) : m_build{ std::forward<F>(build) }
    {}

    [[nodiscard]]
    auto build(TaskHubBuilder& task_hub_builder) && -> TypedTaskFactoryHandle<Result_T>
    {
        return std::move(m_build)(task_hub_builder);
    }

private:
    BuildFunc m_build;
};

export template <typename Result_T>
struct TaskResultTrait<TaskBuilder<Result_T>> {
    using type = Result_T;
};

}   // namespace ddge::scheduler
