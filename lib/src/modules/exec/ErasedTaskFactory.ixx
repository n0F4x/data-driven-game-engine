module;

#include <cassert>
#include <functional>
#include <variant>

export module ddge.modules.exec.ErasedTaskFactory;

import ddge.modules.exec.locks.LockGroup;
import ddge.modules.exec.EmbeddedTaskFactory;
import ddge.modules.exec.IndirectTaskFactory;
import ddge.modules.exec.Task;
import ddge.modules.exec.TaskFactory;
import ddge.modules.exec.TaskHubProxy;

import ddge.utility.any_cast;
import ddge.utility.containers.AnyMoveOnly;

namespace ddge::exec {

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

}   // namespace ddge::exec

template <typename SpecificTaskFactory_T>
struct ErasedTaskFactoryTraits;

template <template <typename> typename SpecificTaskFactory_T, typename Result_T>
struct ErasedTaskFactoryTraits<SpecificTaskFactory_T<Result_T>> {
    [[nodiscard]]
    static auto locks(const ddge::exec::ErasedTaskFactory& that)
        -> const ddge::exec::LockGroup&
    {
        return ddge::util::any_cast<ddge::exec::TaskFactory<Result_T>>(that).visit(
            [](const auto& task_factory) -> const ddge::exec::LockGroup& {
                return task_factory.locks();
            }
        );
    }

    [[nodiscard]]
    static auto build(
        ddge::exec::ErasedTaskFactory&& that,
        const ddge::exec::TaskHubProxy  task_hub_proxy
    ) -> ddge::exec::Task
    {
        SpecificTaskFactory_T<Result_T>* specific_task_factory{
            std::get_if<SpecificTaskFactory_T<Result_T>>(
                &ddge::util::any_cast<ddge::exec::TaskFactory<Result_T>>(that)
            )
        };
        assert(specific_task_factory != nullptr);

        return std::move(*specific_task_factory).build(task_hub_proxy);
    }
};

template <typename Result_T>
ddge::exec::ErasedTaskFactory::ErasedTaskFactory(
    EmbeddedTaskFactory<Result_T>&& embedded_task_factory
)
    : Base{ TaskFactory<Result_T>{ std::move(embedded_task_factory) } },
      m_build{ ErasedTaskFactoryTraits<EmbeddedTaskFactory<Result_T>>::build },
      m_locks{ ErasedTaskFactoryTraits<EmbeddedTaskFactory<Result_T>>::locks }
{}

template <typename Result_T>
ddge::exec::ErasedTaskFactory::ErasedTaskFactory(
    IndirectTaskFactory<Result_T>&& indirect_task_factory
)
    : Base{ TaskFactory<Result_T>{ std::move(indirect_task_factory) } },
      m_build{ ErasedTaskFactoryTraits<IndirectTaskFactory<Result_T>>::build },
      m_locks{ ErasedTaskFactoryTraits<IndirectTaskFactory<Result_T>>::locks }
{}
