module;

#include <functional>

export module ddge.modules.exec.v2.ErasedTaskFactory;

import ddge.modules.exec.v2.EmbeddedTaskFactory;
import ddge.modules.exec.v2.Task;
import ddge.modules.exec.v2.TaskHubProxy;

import ddge.utility.any_cast;
import ddge.utility.containers.AnyMoveOnly;

namespace ddge::exec::v2 {

export class ErasedTaskFactory : public util::BasicAnyMoveOnly<
                                     sizeof(EmbeddedTaskFactory<void>),
                                     alignof(EmbeddedTaskFactory<void>)>   //
{
    using Base = util::BasicAnyMoveOnly<
        sizeof(EmbeddedTaskFactory<void>),
        alignof(EmbeddedTaskFactory<void>)>;

public:
    template <typename Result_T>
    explicit ErasedTaskFactory(EmbeddedTaskFactory<Result_T>&& embedded_task_factory);

    [[nodiscard]]
    auto build(const TaskHubProxy task_hub_proxy) && -> Task
    {
        return m_build(std::move(*this), task_hub_proxy);
    }

private:
    std::reference_wrapper<auto(ErasedTaskFactory&&, TaskHubProxy)->Task> m_build;
};

}   // namespace ddge::exec::v2

template <typename TaskFactory_T>
struct ErasedTaskFactoryTraits {
    [[nodiscard]]
    static auto build(
        ddge::exec::v2::ErasedTaskFactory&& that,
        const ddge::exec::v2::TaskHubProxy  task_hub_proxy
    ) -> ddge::exec::v2::Task
    {
        return ddge::util::any_cast<TaskFactory_T>(std::move(that)).build(task_hub_proxy);
    }
};

template <typename Result_T>
ddge::exec::v2::ErasedTaskFactory::ErasedTaskFactory(
    EmbeddedTaskFactory<Result_T>&& embedded_task_factory
)
    : Base{ std::move(embedded_task_factory) },
      m_build{ ErasedTaskFactoryTraits<EmbeddedTaskFactory<Result_T>>::build }
{}
