module;

#include <type_traits>
#include <utility>

export module ddge.modules.scheduler.TaskBlueprint;

import ddge.modules.scheduler.Nexus;
import ddge.modules.scheduler.Cardinality;
import ddge.modules.scheduler.TaskHubBuilder;
import ddge.modules.scheduler.TaskHubProxy;
import ddge.modules.scheduler.TaskBuilder;
import ddge.modules.scheduler.TaskBuilderBundle;

import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::scheduler {

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

}   // namespace ddge::scheduler
