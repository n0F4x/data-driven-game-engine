module;

#include <cstddef>
#include <utility>

export module modules.scheduler.accessors.ecs.Query;

import modules.ecs;

namespace modules::scheduler::accessors {

inline namespace ecs {

export template <modules::ecs::query_parameter_c... Parameters_T>
    requires(sizeof...(Parameters_T) != 0)
class Query {
public:
    explicit Query(modules::ecs::Registry& registry);
    Query(const Query&) = delete("Queries should be taken by reference");
    Query(Query&&)      = default;

    template <typename F>
    auto for_each(F&& func) -> F;

    [[nodiscard]]
    auto count() -> std::size_t;

private:
    modules::ecs::Query<Parameters_T...> m_query;
};

}   // namespace ecs

}   // namespace modules::scheduler::accessors

template <modules::ecs::query_parameter_c... Parameters_T>
    requires(sizeof...(Parameters_T) != 0)
modules::scheduler::accessors::ecs::Query<Parameters_T...>::Query(
    modules::ecs::Registry& registry
)
    : m_query{ registry }
{}

template <modules::ecs::query_parameter_c... Parameters_T>
    requires(sizeof...(Parameters_T) != 0)
template <typename F>
auto modules::scheduler::accessors::ecs::Query<Parameters_T...>::for_each(F&& func)
    -> F
{
    return m_query(std::forward<F>(func));
}

template <modules::ecs::query_parameter_c... Parameters_T>
    requires(sizeof...(Parameters_T) != 0)
auto modules::scheduler::accessors::Query<Parameters_T...>::count() -> std::size_t
{
    return m_query.count();
}
