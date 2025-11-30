module;

#include <optional>
#include <utility>

export module ddge.modules.scheduler.EmbeddedTaskFactory;

import ddge.modules.scheduler.locks.LockGroup;
import ddge.modules.scheduler.EmbeddedTaskBody;
import ddge.modules.scheduler.Task;
import ddge.modules.scheduler.TaskContinuationFactory;
import ddge.modules.scheduler.TaskHubProxy;

namespace ddge::scheduler {

export template <typename Result_T>
class EmbeddedTaskFactory {
public:
    explicit EmbeddedTaskFactory(EmbeddedTaskBody<Result_T>&& body, LockGroup&& locks)
        : m_body{ std::move(body) },
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
        return m_continuation_factory.has_value()
                 ? Task{ [body         = std::move(m_body),
                          continuation = std::move(*m_continuation_factory)(
                              task_hub_proxy
                          )] mutable -> void   //
                         {
                             if constexpr (std::is_void_v<Result_T>) {
                                 body();
                                 continuation();
                             }
                             else {
                                 continuation(body());
                             }
                         } }
                 : Task{ std::move(m_body) };
    }

private:
    EmbeddedTaskBody<Result_T>                       m_body;
    LockGroup                                        m_locks;
    std::optional<TaskContinuationFactory<Result_T>> m_continuation_factory;
};

}   // namespace ddge::scheduler
