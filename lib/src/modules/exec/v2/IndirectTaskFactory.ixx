module;

#include <optional>
#include <utility>
#include <vector>

export module ddge.modules.exec.v2.IndirectTaskFactory;

import ddge.modules.exec.v2.IndirectTaskContinuationSetter;
import ddge.modules.exec.v2.Task;
import ddge.modules.exec.v2.TaskContinuationFactory;
import ddge.modules.exec.v2.TaskHubProxy;
import ddge.modules.exec.v2.TaskIndex;

import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::exec::v2 {

export template <typename Result_T>
class IndirectTaskFactory {
public:
    explicit IndirectTaskFactory(
        const TaskIndex                            start_up_index,
        IndirectTaskContinuationSetter<Result_T>&& set_task_continuation
    )
        : IndirectTaskFactory{ std::vector<TaskIndex>{ start_up_index },
                               std::move(set_task_continuation) }
    {}

    explicit IndirectTaskFactory(
        std::vector<TaskIndex>&&                   start_up_indices,
        IndirectTaskContinuationSetter<Result_T>&& set_task_continuation
    )
        : m_start_up_indices{ std::move(start_up_indices) },
          m_set_task_continuation{ std::move(set_task_continuation) }
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
    std::optional<TaskContinuationFactory<Result_T>> m_continuation_factory;
};

}   // namespace ddge::exec::v2
