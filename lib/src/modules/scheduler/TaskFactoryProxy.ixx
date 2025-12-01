module;

#include <functional>

export module ddge.modules.scheduler.TaskFactoryProxy;

import ddge.modules.scheduler.locks.LockGroup;
import ddge.modules.scheduler.TaskContinuationFactory;
import ddge.modules.scheduler.TaskFactory;
import ddge.modules.scheduler.TypedTaskIndex;

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
        m_task_factory_ref.get().visit([&continuation_factory](auto& task_factory) -> void {
            task_factory.set_continuation_factory(std::move(continuation_factory));
        });
    }

    [[nodiscard]]
    auto locks() const -> const LockGroup&
    {
        return m_task_factory_ref.get().visit(
            [](const auto& task_factory) -> const LockGroup& {
                return task_factory.locks();
            }
        );
    }

private:
    std::reference_wrapper<TaskFactory<Result_T>> m_task_factory_ref;
};

}   // namespace ddge::scheduler
