module;

#include <cstddef>
#include <utility>

export module extensions.scheduler.accessors.ecs.Query;

import core.ecs;

namespace extensions::scheduler::accessors {

inline namespace ecs {

export template <core::ecs::query_parameter_c... Parameters_T>
    requires(sizeof...(Parameters_T) != 0)
class Query {
public:
    explicit Query(core::ecs::Registry& registry);
    Query(const Query&) = delete("Queries should be taken by reference");
    Query(Query&&)      = default;

    template <typename F>
    auto for_each(F&& func) -> F;

    [[nodiscard]]
    auto count() -> std::size_t;

private:
    core::ecs::Query<Parameters_T...> m_query;
};

}   // namespace ecs

}   // namespace extensions::scheduler::accessors

template <core::ecs::query_parameter_c... Parameters_T>
    requires(sizeof...(Parameters_T) != 0)
extensions::scheduler::accessors::ecs::Query<Parameters_T...>::Query(
    core::ecs::Registry& registry
)
    : m_query{ registry }
{}

template <core::ecs::query_parameter_c... Parameters_T>
    requires(sizeof...(Parameters_T) != 0)
template <typename F>
auto extensions::scheduler::accessors::ecs::Query<Parameters_T...>::for_each(F&& func)
    -> F
{
    return m_query(std::forward<F>(func));
}

template <core::ecs::query_parameter_c... Parameters_T>
    requires(sizeof...(Parameters_T) != 0)
auto extensions::scheduler::accessors::Query<Parameters_T...>::count() -> std::size_t
{
    return m_query.count();
}
