module;

#include <cassert>
#include <functional>
#include <variant>

export module ddge.modules.exec.v2.ErasedTaskFactory;

import ddge.modules.exec.v2.EmbeddedTaskFactory;
import ddge.modules.exec.v2.Task;
import ddge.modules.exec.v2.TaskFactory;
import ddge.modules.exec.v2.TaskHubProxy;

import ddge.utility.any_cast;
import ddge.utility.containers.AnyMoveOnly;

namespace ddge::exec::v2 {

export class ErasedTaskFactory : public util::BasicAnyMoveOnly<
                                     sizeof(TaskFactory<void>),
                                     alignof(TaskFactory<void>)>   //
{
    using Base =
        util::BasicAnyMoveOnly<sizeof(TaskFactory<void>), alignof(TaskFactory<void>)>;

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

template <typename SpecificTaskFactory_T>
struct ErasedTaskFactoryTraits;

template <template <typename> typename SpecificTaskFactory_T, typename Result_T>
struct ErasedTaskFactoryTraits<SpecificTaskFactory_T<Result_T>> {
    [[nodiscard]]
    static auto build(
        ddge::exec::v2::ErasedTaskFactory&& that,
        const ddge::exec::v2::TaskHubProxy  task_hub_proxy
    ) -> ddge::exec::v2::Task
    {
        SpecificTaskFactory_T<Result_T>* specific_task_factory{
            std::get_if<SpecificTaskFactory_T<Result_T>>(
                &ddge::util::any_cast<ddge::exec::v2::TaskFactory<Result_T>>(that)
            )
        };
        assert(specific_task_factory != nullptr);

        return std::move(*specific_task_factory).build(task_hub_proxy);
    }
};

template <typename Result_T>
ddge::exec::v2::ErasedTaskFactory::ErasedTaskFactory(
    EmbeddedTaskFactory<Result_T>&& embedded_task_factory
)
    : Base{ TaskFactory<Result_T>{ std::move(embedded_task_factory) } },
      m_build{ ErasedTaskFactoryTraits<EmbeddedTaskFactory<Result_T>>::build }
{}
