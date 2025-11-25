module;

#include <type_traits>
#include <utility>

export module ddge.modules.exec.v2.TaskBlueprint;

import ddge.modules.exec.Nexus;
import ddge.modules.exec.v2.Cardinality;
import ddge.modules.exec.v2.TaskFinishedCallback;
import ddge.modules.exec.v2.TaskHubBuilder;
import ddge.modules.exec.v2.TaskHubProxy;
import ddge.modules.exec.v2.TaskBuilder;
import ddge.modules.exec.v2.TaskBuilderBundle;

import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::exec::v2 {

export template <typename Result_T, Cardinality cardinality_T>
class TaskBlueprint {
    using Materialized = std::conditional_t<
        cardinality_T == Cardinality::eSingle,
        TaskBuilder<Result_T>,
        TaskBuilderBundle<Result_T>>;

    using MaterializeFunc = util::AnyMoveOnlyFunction<auto()&&->Materialized>;

public:
    constexpr static std::integral_constant<Cardinality, cardinality_T> cardinality;

    template <typename F>
    explicit TaskBlueprint(F&& materialize)
        : m_materialize{ std::forward<F>(materialize) }
    {}

    [[nodiscard]]
    auto materialize() && -> Materialized
    {
        return std::move(m_materialize)();
    }

private:
    MaterializeFunc m_materialize;
};

}   // namespace ddge::exec::v2
