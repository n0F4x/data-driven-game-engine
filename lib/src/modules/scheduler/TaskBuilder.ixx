module;

#include <utility>

export module ddge.modules.scheduler.TaskBuilder;

import ddge.modules.scheduler.TaskHubBuilder;
import ddge.modules.scheduler.TaskResultTrait;
import ddge.modules.scheduler.TypedTaskFactoryHandle;

import ddge.util.containers.MoveOnlyFunction;

namespace ddge::scheduler {

template <typename Result_T>
using BuildFunc =
    util::MoveOnlyFunction<TypedTaskFactoryHandle<Result_T>(TaskHubBuilder&) &&>;

export template <typename Result_T>
class TaskBuilder {
public:
    template <typename F>
        requires(!std::same_as<std::remove_cvref_t<F>, TaskBuilder>)
    explicit TaskBuilder(F&& build);

    [[nodiscard]]
    auto build(TaskHubBuilder& task_hub_builder) && -> TypedTaskFactoryHandle<Result_T>;

private:
    BuildFunc<Result_T> m_build;
};

export template <typename Result_T>
struct TaskResultTrait<TaskBuilder<Result_T>> {
    using type = Result_T;
};

}   // namespace ddge::scheduler

namespace ddge::scheduler {

template <typename Result_T>
template <typename F>
    requires(!std::same_as<std::remove_cvref_t<F>, TaskBuilder<Result_T>>)
TaskBuilder<Result_T>::TaskBuilder(F&& build) : m_build{ std::forward<F>(build) }
{}

template <typename Result_T>
auto TaskBuilder<Result_T>::build(
    TaskHubBuilder& task_hub_builder
) && -> TypedTaskFactoryHandle<Result_T>
{
    return std::move(m_build)(task_hub_builder);
}

}   // namespace ddge::scheduler
