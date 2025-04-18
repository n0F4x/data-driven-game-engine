module;

#include <functional>

export module core.scheduler.Task;

import utility.meta.concepts.functional.function;
import utility.meta.concepts.functional.member_function_pointer;
import utility.meta.concepts.functional.unambiguous_functor;
import utility.meta.type_traits.functional.arguments_of;
import utility.meta.type_traits.functional.result_of;

namespace core::scheduler {

export template <typename Task_T>
class Task;

template <typename ArgumentsTypeList_T, typename Result_T>
class TaskBase;

template <
    template <typename...> typename ArgumentsTypeList_T,
    typename... Arguments_T,
    typename Result_T>
class TaskBase<ArgumentsTypeList_T<Arguments_T...>, Result_T> {
public:
    using Arguments = ArgumentsTypeList_T<Arguments_T...>;
    using Result    = Result_T;

    template <typename Self_T>
    constexpr auto operator()(this Self_T&&, Arguments_T... args) -> Result_T;
};

template <typename Task_T>
class TaskInterface
    : public TaskBase<util::meta::arguments_of_t<Task_T>, util::meta::result_of_t<Task_T>> {
};

template <typename Task_T>
    requires util::meta::function_c<Task_T>
class Task<Task_T> : public TaskInterface<Task_T> {
public:
    constexpr explicit Task(Task_T& task);

protected:
    std::reference_wrapper<Task_T> m_task;
};

template <typename Task_T>
    requires util::meta::member_function_pointer_c<Task_T>
class Task<Task_T> : public TaskInterface<Task_T> {
public:
    constexpr explicit Task(Task_T task);

protected:
    Task_T m_task;
};

template <typename Task_T>
    requires util::meta::unambiguous_functor_c<Task_T>
class Task<Task_T> : public TaskInterface<Task_T> {
public:
    template <typename UTask_T>
        requires std::constructible_from<Task_T, UTask_T>
    constexpr explicit Task(UTask_T&& task);

protected:
    Task_T m_task;
};

}   // namespace core::scheduler

template <
    template <typename...> class ArgumentsTypeList_T,
    typename... Arguments_T,
    typename Result_T>
template <typename Self_T>
constexpr auto core::scheduler::TaskBase<ArgumentsTypeList_T<Arguments_T...>, Result_T>::
    operator()(this Self_T&& self, Arguments_T... args) -> Result_T
{
    return std::invoke(self.m_task, std::forward<Arguments_T>(args)...);
}

template <typename Task_T>
    requires util::meta::function_c<Task_T>
constexpr core::scheduler::Task<Task_T>::Task(Task_T& task) : m_task{ task }
{}

template <typename Task_T>
    requires util::meta::member_function_pointer_c<Task_T>
constexpr core::scheduler::Task<Task_T>::Task(Task_T task) : m_task{ task }
{}

template <typename Task_T>
    requires util::meta::unambiguous_functor_c<Task_T>
template <typename UTask_T>
    requires std::constructible_from<Task_T, UTask_T>
constexpr core::scheduler::Task<Task_T>::Task(UTask_T&& task)
    : m_task{ std::forward<UTask_T>(task) }
{}
