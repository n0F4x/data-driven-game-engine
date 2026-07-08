module;

#include <functional>
#include <variant>

export module ddge.scheduler.TaskFactoryProxy;

import ddge.scheduler.locks.LockGroup;
import ddge.scheduler.TaskContinuationFactory;
import ddge.scheduler.TaskFactory;
import ddge.scheduler.TypedTaskIndex;

namespace ddge::scheduler {

export template <typename Result_T>
class TaskFactoryProxy {
public:
    explicit TaskFactoryProxy(TaskFactory<Result_T>& task_factory)
        : m_task_factory_ref{ task_factory }
    {}

    auto set_continuation_factory(TaskContinuationFactory<Result_T>&& continuation_factory)
        -> void
    {
        std::visit(
            [&continuation_factory](auto& task_factory) -> void {
                task_factory.set_continuation_factory(std::move(continuation_factory));
            },
            m_task_factory_ref.get()
        );
    }

    [[nodiscard]]
    auto locks() const -> const LockGroup&
    {
        return std::visit(
            [](const auto& task_factory) -> const LockGroup& {
                return task_factory.locks();
            },
            m_task_factory_ref.get()
        );
    }

private:
    std::reference_wrapper<TaskFactory<Result_T>> m_task_factory_ref;
};

}   // namespace ddge::scheduler
