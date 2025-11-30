module;

#include <type_traits>
#include <utility>

export module ddge.modules.exec.TaskBlueprint;

import ddge.modules.exec.Nexus;
import ddge.modules.exec.Cardinality;
import ddge.modules.exec.TaskHubBuilder;
import ddge.modules.exec.TaskHubProxy;
import ddge.modules.exec.TaskBuilder;
import ddge.modules.exec.TaskBuilderBundle;

import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::exec {

export template <typename Result_T, Cardinality cardinality_T>
class TaskBlueprint {
    using Materialized = std::conditional_t<
        cardinality_T == Cardinality::eSingle,
        TaskBuilder<Result_T>,
        TaskBuilderBundle<Result_T>>;

    using MaterializeFunc = util::AnyMoveOnlyFunction<auto() &&->Materialized>;

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

}   // namespace ddge::exec
