module;

#include <optional>
#include <utility>

export module ddge.modules.exec.v2.IndirectTaskFactory;

import ddge.modules.exec.v2.IndirectTaskBody;
import ddge.modules.exec.v2.IndirectTaskContinuationSetter;
import ddge.modules.exec.v2.Task;
import ddge.modules.exec.v2.TaskContinuationFactory;
import ddge.modules.exec.v2.TaskHubProxy;

import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::exec::v2 {

export template <typename Result_T>
class IndirectTaskFactory {
public:
    explicit IndirectTaskFactory(
        IndirectTaskBody&&                         body,
        IndirectTaskContinuationSetter<Result_T>&& set_task_continuation
    )
        : m_body{ std::move(body) },
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
            [body = std::move(m_body), task_hub_proxy] mutable -> void {
                body(task_hub_proxy);
            }   //
        };
    }

private:
    IndirectTaskBody                                 m_body;
    IndirectTaskContinuationSetter<Result_T>         m_set_task_continuation;
    std::optional<TaskContinuationFactory<Result_T>> m_continuation_factory;
};

}   // namespace ddge::exec::v2
