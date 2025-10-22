module;

#include <type_traits>

#include <function2/function2.hpp>

export module ddge.modules.execution.v2.TaskBlueprint;

import ddge.modules.execution.Nexus;
import ddge.modules.execution.v2.Cardinality;
import ddge.modules.execution.v2.TaskFinishedCallback;
import ddge.modules.execution.v2.TaskHubBuilder;
import ddge.modules.execution.v2.TaskHubProxy;
import ddge.modules.execution.v2.TaskBuilder;
import ddge.modules.execution.v2.TaskBuilderBundle;

namespace ddge::exec::v2 {

template <typename Result_T, Cardinality cardinality_T>
using TaskBlueprintUnderlyingFunctionResult = std::conditional_t<
    cardinality_T == Cardinality::eSingle,
    TaskBuilder<Result_T>,
    TaskBuilderBundle<Result_T>>;

template <typename Result_T, Cardinality cardinality_T>
using TaskBlueprintUnderlyingFunction =
    fu2::unique_function<auto()&&->TaskBlueprintUnderlyingFunctionResult<Result_T, cardinality_T>>;

export template <typename Result_T, Cardinality cardinality_T>
class TaskBlueprint : TaskBlueprintUnderlyingFunction<Result_T, cardinality_T> {
public:
    constexpr static std::integral_constant<Cardinality, cardinality_T> cardinality;

    using Materialized = TaskBlueprintUnderlyingFunctionResult<Result_T, cardinality_T>;

    using TaskBlueprintUnderlyingFunction<Result_T, cardinality_T>::TaskBlueprintUnderlyingFunction;

    [[nodiscard]]
    auto materialize() && -> Materialized
    {
        return std::move(*this)();
    }
};

}   // namespace ddge::exec::v2
