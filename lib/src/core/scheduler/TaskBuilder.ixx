module;

#include <concepts>
#include <functional>

export module core.scheduler.TaskBuilder;

import core.scheduler.Nexus;
import core.scheduler.Task;

import utility.meta.concepts.naked;

namespace core::scheduler {

export template <typename Result_T>
class TaskBuilder {
public:
    using Result = Result_T;

    template <std::regular_invocable<Nexus&> BuildFunction_T>
        requires std::
            same_as<std::invoke_result_t<BuildFunction_T, Nexus&>, Task<Result_T>>
        explicit TaskBuilder(BuildFunction_T&& build_function);

    [[nodiscard]]
    auto build(Nexus& nexus) const -> Task<Result_T>;

private:
    std::function<Task<Result_T>(Nexus&)> m_build;
};

template <typename>
struct IsSpecializationOfTaskBuilder : std::false_type {};

template <typename Result_T>
struct IsSpecializationOfTaskBuilder<TaskBuilder<Result_T>> : std::true_type {};

export template <typename T>
concept specialization_of_TaskBuilder_c = util::meta::naked_c<T>
                                       && IsSpecializationOfTaskBuilder<T>::value;

}   // namespace core::scheduler

template <typename Result_T>
template <std::regular_invocable<core::scheduler::Nexus&> BuildFunction_T>
    requires std::same_as<
        std::invoke_result_t<BuildFunction_T, core::scheduler::Nexus&>,
        core::scheduler::Task<Result_T>>
core::scheduler::TaskBuilder<Result_T>::TaskBuilder(BuildFunction_T&& build_function)
    : m_build{ std::forward<BuildFunction_T>(build_function) }
{}

template <typename Result_T>
auto core::scheduler::TaskBuilder<Result_T>::build(Nexus& nexus) const
    -> Task<Result_T>
{
    return m_build(nexus);
}
