module;

#include <functional>
#include <string>
#include <string_view>
#include <tuple>

export module core.scheduler.Task;

import utility.meta.algorithms.apply;
import utility.meta.concepts.functional.function;
import utility.meta.concepts.functional.member_function_pointer;
import utility.meta.concepts.functional.unambiguous_functor;
import utility.meta.reflection.name_of;
import utility.meta.type_traits.functional.arguments_of;
import utility.meta.type_traits.functional.result_of;
import utility.meta.type_traits.type_list.type_list_transform;

import core.scheduler.concepts.provides_dependency_c;

namespace core::scheduler {

export template <typename Task_T>
class Task;

class TaskBase {};

template <typename Task_T>
class TaskInterface : public TaskBase {
public:
    using Dependencies =
        util::meta::type_list_transform_t<util::meta::arguments_of_t<Task_T>, std::decay>;
    using Result = util::meta::result_of_t<Task_T>;

    template <typename Self_T, typename... DependencyProviders_T>
    constexpr auto
        operator()(this Self_T&&, DependencyProviders_T&&... dependency_providers)
            -> Result;
};

template <typename Task_T>
    requires util::meta::function_c<Task_T>
class Task<Task_T> : public TaskInterface<Task_T> {
public:
    constexpr explicit Task(Task_T& task);

private:
    friend TaskInterface<Task_T>;

    std::reference_wrapper<Task_T> m_task;
};

template <typename Task_T>
    requires util::meta::member_function_pointer_c<Task_T>
class Task<Task_T> : public TaskInterface<Task_T> {
public:
    constexpr explicit Task(Task_T task);

private:
    friend TaskInterface<Task_T>;

    Task_T m_task;
};

template <typename Task_T>
    requires util::meta::unambiguous_functor_c<Task_T>
class Task<Task_T> : public TaskInterface<Task_T> {
public:
    template <typename UTask_T>
        requires std::constructible_from<Task_T, UTask_T&&>
    constexpr explicit Task(UTask_T&& task);

private:
    friend TaskInterface<Task_T>;

    Task_T m_task;
};

}   // namespace core::scheduler

template <typename Task_T>
template <typename Self_T, typename... DependencyProviders_T>
constexpr auto core::scheduler::TaskInterface<Task_T>::operator()(
    this Self_T&& self,
    DependencyProviders_T&&... dependency_providers
) -> Result
{
    const auto provide_dependency_for =
        [&dependency_providers...]<size_t provider_index_T, typename Dependency_T>(
            this auto fn_self
        ) {
            using namespace std::literals;
            static_assert(
                provider_index_T < sizeof...(DependencyProviders_T),
                "no provider found for dependency `"s
                    + util::meta::name_of<Dependency_T>() + "`"sv
            );

            if constexpr (provides_dependency_c<
                              DependencyProviders_T...[provider_index_T],
                              Dependency_T>)
            {
                return dependency_providers...[provider_index_T]
                    .template provide<Dependency_T>();
            }
            else {
                return fn_self.template operator()<provider_index_T + 1, Dependency_T>();
            }
        };

    const auto collect_dependencies = [&provide_dependency_for] {
        return util::meta::apply<Dependencies>(
            [&provide_dependency_for]<typename... Arguments_T> {
                return std::make_tuple(provide_dependency_for.template operator(
                )<0, Arguments_T>()...);
            }
        );
    };

    return std::apply(self.m_task, collect_dependencies());
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
    requires std::constructible_from<Task_T, UTask_T&&>
constexpr core::scheduler::Task<Task_T>::Task(UTask_T&& task)
    : m_task{ std::forward<UTask_T>(task) }
{}
