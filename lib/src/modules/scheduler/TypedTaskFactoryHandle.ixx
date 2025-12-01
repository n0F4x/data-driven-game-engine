export module ddge.modules.scheduler.TypedTaskFactoryHandle;

import ddge.modules.scheduler.TaskFactoryProxy;
import ddge.modules.scheduler.TypedTaskIndex;

namespace ddge::scheduler {

export template <typename Result_T>
class TypedTaskFactoryHandle {
public:
    explicit TypedTaskFactoryHandle(
        const TaskFactoryProxy<Result_T> task_factory_proxy,
        const TypedTaskIndex<Result_T>   task_index
    )
        : m_task_factory_proxy{ task_factory_proxy },
          m_index{ task_index }
    {}

    auto operator->() const noexcept -> TaskFactoryProxy<Result_T>*
    {
        return &m_task_factory_proxy;
    }

    [[nodiscard]]
    auto index() const noexcept -> TypedTaskIndex<Result_T>
    {
        return m_index;
    }

private:
    mutable TaskFactoryProxy<Result_T> m_task_factory_proxy;
    TypedTaskIndex<Result_T>           m_index;
};

}   // namespace ddge::scheduler
