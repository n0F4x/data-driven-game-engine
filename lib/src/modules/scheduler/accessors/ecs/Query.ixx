module;

#include <cstddef>
#include <utility>

export module ddge.modules.scheduler.accessors.ecs.Query;

import ddge.modules.ecs;

namespace ddge::scheduler::accessors {

inline namespace ecs {

export template <ddge::ecs::query_parameter_c... Parameters_T>
    requires(sizeof...(Parameters_T) != 0)
class Query {
public:
    explicit Query(ddge::ecs::Registry& registry);
    Query(const Query&) = delete("Queries should be taken by reference");
    Query(Query&&)      = default;

    template <typename F>
    auto for_each(F&& func) -> F;

    [[nodiscard]]
    auto count() -> std::size_t;

private:
    ddge::ecs::Query<Parameters_T...> m_query;
};

}   // namespace ecs

}   // namespace ddge::scheduler::accessors

template <ddge::ecs::query_parameter_c... Parameters_T>
    requires(sizeof...(Parameters_T) != 0)
ddge::scheduler::accessors::ecs::Query<Parameters_T...>::Query(
    ddge::ecs::Registry& registry
)
    : m_query{ registry }
{}

template <ddge::ecs::query_parameter_c... Parameters_T>
    requires(sizeof...(Parameters_T) != 0)
template <typename F>
auto ddge::scheduler::accessors::ecs::Query<Parameters_T...>::for_each(F&& func) -> F
{
    return m_query(std::forward<F>(func));
}

template <ddge::ecs::query_parameter_c... Parameters_T>
    requires(sizeof...(Parameters_T) != 0)
auto ddge::scheduler::accessors::Query<Parameters_T...>::count() -> std::size_t
{
    return m_query.count();
}
