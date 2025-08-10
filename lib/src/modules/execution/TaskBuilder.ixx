module;

#include <concepts>
#include <functional>

export module ddge.modules.execution.TaskBuilder;

import ddge.modules.execution.Nexus;
import ddge.modules.execution.Task;

import ddge.utility.meta.concepts.naked;

namespace ddge::exec {

export template <typename Result_T>
class TaskBuilder {
public:
    using Result = Result_T;

    template <std::regular_invocable<Nexus&> BuildFunction_T>
        requires std::same_as<std::invoke_result_t<BuildFunction_T, Nexus&>, Task<Result_T>>
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

}   // namespace ddge::exec

template <typename Result_T>
template <std::regular_invocable<ddge::exec::Nexus&> BuildFunction_T>
    requires std::same_as<
        std::invoke_result_t<BuildFunction_T, ddge::exec::Nexus&>,
        ddge::exec::Task<Result_T>>
ddge::exec::TaskBuilder<Result_T>::TaskBuilder(BuildFunction_T&& build_function)
    : m_build{ std::forward<BuildFunction_T>(build_function) }
{}

template <typename Result_T>
auto ddge::exec::TaskBuilder<Result_T>::build(Nexus& nexus) const -> Task<Result_T>
{
    return m_build(nexus);
}
