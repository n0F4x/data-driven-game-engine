module;

#include <cassert>
#include <functional>
#include <variant>

export module ddge.modules.scheduler.ErasedTaskFactory;

import ddge.modules.scheduler.locks.LockGroup;
import ddge.modules.scheduler.EmbeddedTaskFactory;
import ddge.modules.scheduler.IndirectTaskFactory;
import ddge.modules.scheduler.Task;
import ddge.modules.scheduler.TaskFactory;
import ddge.modules.scheduler.TaskHubProxy;

import ddge.utility.any_cast;
import ddge.utility.containers.AnyMoveOnly;

namespace ddge::scheduler {

export class ErasedTaskFactory : public util::BasicAnyMoveOnly<
                                     sizeof(TaskFactory<void>),
                                     alignof(TaskFactory<void>)>   //
{
    using Base =
        util::BasicAnyMoveOnly<sizeof(TaskFactory<void>), alignof(TaskFactory<void>)>;

public:
    template <typename Result_T>
    explicit ErasedTaskFactory(EmbeddedTaskFactory<Result_T>&& embedded_task_factory);

    template <typename Result_T>
    explicit ErasedTaskFactory(IndirectTaskFactory<Result_T>&& indirect_task_factory);

    [[nodiscard]]
    auto locks() const noexcept -> const LockGroup&
    {
        return m_locks(*this);
    }

    [[nodiscard]]
    auto build(const TaskHubProxy task_hub_proxy) && -> Task
    {
        return m_build(std::move(*this), task_hub_proxy);
    }

private:
    std::reference_wrapper<auto(ErasedTaskFactory&&, TaskHubProxy)->Task>    m_build;
    std::reference_wrapper<auto(const ErasedTaskFactory&)->const LockGroup&> m_locks;
};

}   // namespace ddge::scheduler

template <typename SpecificTaskFactory_T>
struct ErasedTaskFactoryTraits;

template <template <typename> typename SpecificTaskFactory_T, typename Result_T>
struct ErasedTaskFactoryTraits<SpecificTaskFactory_T<Result_T>> {
    [[nodiscard]]
    static auto locks(const ddge::scheduler::ErasedTaskFactory& that)
        -> const ddge::scheduler::LockGroup&
    {
        return ddge::util::any_cast<ddge::scheduler::TaskFactory<Result_T>>(that).visit(
            [](const auto& task_factory) -> const ddge::scheduler::LockGroup& {
                return task_factory.locks();
            }
        );
    }

    [[nodiscard]]
    static auto build(
        ddge::scheduler::ErasedTaskFactory&& that,
        const ddge::scheduler::TaskHubProxy  task_hub_proxy
    ) -> ddge::scheduler::Task
    {
        SpecificTaskFactory_T<Result_T>* specific_task_factory{
            std::get_if<SpecificTaskFactory_T<Result_T>>(
                &ddge::util::any_cast<ddge::scheduler::TaskFactory<Result_T>>(that)
            )
        };
        assert(specific_task_factory != nullptr);

        return std::move(*specific_task_factory).build(task_hub_proxy);
    }
};

template <typename Result_T>
ddge::scheduler::ErasedTaskFactory::ErasedTaskFactory(
    EmbeddedTaskFactory<Result_T>&& embedded_task_factory
)
    : Base{ TaskFactory<Result_T>{ std::move(embedded_task_factory) } },
      m_build{ ErasedTaskFactoryTraits<EmbeddedTaskFactory<Result_T>>::build },
      m_locks{ ErasedTaskFactoryTraits<EmbeddedTaskFactory<Result_T>>::locks }
{}

template <typename Result_T>
ddge::scheduler::ErasedTaskFactory::ErasedTaskFactory(
    IndirectTaskFactory<Result_T>&& indirect_task_factory
)
    : Base{ TaskFactory<Result_T>{ std::move(indirect_task_factory) } },
      m_build{ ErasedTaskFactoryTraits<IndirectTaskFactory<Result_T>>::build },
      m_locks{ ErasedTaskFactoryTraits<IndirectTaskFactory<Result_T>>::locks }
{}
