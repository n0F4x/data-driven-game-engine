module;

#include <utility>

#include <function2/function2.hpp>

export module ddge.modules.exec.v2.TaskBuilder;

import ddge.modules.exec.Nexus;
import ddge.modules.exec.v2.TaskFinishedCallback;
import ddge.modules.exec.v2.TaskHubBuilder;
import ddge.modules.exec.v2.TaskBundle;

namespace ddge::exec::v2 {

template <typename Result_T>
using UnderylingFunction = fu2::unique_function<
    TaskBundle(Nexus&, TaskHubBuilder&, TaskFinishedCallback<Result_T>&&) &&>;

export template <typename Result_T>
class TaskBuilder : UnderylingFunction<Result_T> {
public:
    using UnderylingFunction<Result_T>::UnderylingFunction;

    [[nodiscard]]
    auto build(
        Nexus&                           nexus,
        TaskHubBuilder&                  task_hub_builder,
        TaskFinishedCallback<Result_T>&& callback
    ) && -> TaskBundle
    {
        return std::move(*this)(nexus, task_hub_builder, std::move(callback));
    }
};

}   // namespace ddge::exec::v2
