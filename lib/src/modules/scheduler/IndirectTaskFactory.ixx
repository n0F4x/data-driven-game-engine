module;

#include <optional>
#include <utility>
#include <vector>

export module ddge.modules.scheduler.IndirectTaskFactory;

import ddge.modules.scheduler.locks.LockGroup;
import ddge.modules.scheduler.IndirectTaskContinuationSetter;
import ddge.modules.scheduler.Task;
import ddge.modules.scheduler.TaskContinuationFactory;
import ddge.modules.scheduler.TaskHubProxy;
import ddge.modules.scheduler.TaskIndex;

import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::scheduler {

export template <typename Result_T>
class IndirectTaskFactory {
public:
    explicit IndirectTaskFactory(
        const TaskIndex                            start_up_index,
        IndirectTaskContinuationSetter<Result_T>&& set_task_continuation,
        LockGroup&&                                locks
    )
        : IndirectTaskFactory{ std::vector<TaskIndex>{ start_up_index },
                               std::move(set_task_continuation),
                               std::move(locks) }
    {}

    explicit IndirectTaskFactory(
        std::vector<TaskIndex>&&                   start_up_indices,
        IndirectTaskContinuationSetter<Result_T>&& set_task_continuation,
        LockGroup&&                                locks
    )
        : m_start_up_indices{ std::move(start_up_indices) },
          m_set_task_continuation{ std::move(set_task_continuation) },
          m_locks{ std::move(locks) }
    {}

    template <typename Self_T>
    auto set_continuation_factory(
        this Self_T&&                       self,
        TaskContinuationFactory<Result_T>&& continuation_factory
    ) -> Self_T
    {
        self.m_continuation_factory = std::move(continuation_factory);
        return std::forward<Self_T>(self);
    }

    [[nodiscard]]
    auto locks() const noexcept -> const LockGroup&
    {
        return m_locks;
    }

    [[nodiscard]]
    auto build(const TaskHubProxy task_hub_proxy) && -> Task
    {
        if (m_continuation_factory.has_value()) {
            std::move(m_set_task_continuation)(
                std::move(*m_continuation_factory)(task_hub_proxy)
            );
        }

        return Task{
            [start_up_indices = std::move(m_start_up_indices),
             task_hub_proxy] mutable -> void {
                for (const TaskIndex task_index : start_up_indices) {
                    task_hub_proxy.schedule(task_index);
                }
            }   //
        };
    }

private:
    // TODO: use SBO
    std::vector<TaskIndex>                           m_start_up_indices;
    IndirectTaskContinuationSetter<Result_T>         m_set_task_continuation;
    LockGroup                                        m_locks;
    std::optional<TaskContinuationFactory<Result_T>> m_continuation_factory;
};

}   // namespace ddge::scheduler
