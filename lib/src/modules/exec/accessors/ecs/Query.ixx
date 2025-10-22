module;

#include <cstddef>
#include <utility>

export module ddge.modules.exec.accessors.ecs.Query;

import ddge.modules.ecs;

namespace ddge::exec::accessors {

inline namespace ecs {

export template <ddge::ecs::query_filter_c... Filters_T>
    requires(sizeof...(Filters_T) != 0)
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
    ddge::ecs::Query<Filters_T...> m_query;
};

}   // namespace ecs

}   // namespace ddge::exec::accessors

template <ddge::ecs::query_filter_c... Filters_T>
    requires(sizeof...(Filters_T) != 0)
ddge::exec::accessors::ecs::Query<Filters_T...>::Query(ddge::ecs::Registry& registry)
    : m_query{ registry }
{}

template <ddge::ecs::query_filter_c... Filters_T>
    requires(sizeof...(Filters_T) != 0)
template <typename F>
auto ddge::exec::accessors::ecs::Query<Filters_T...>::for_each(F&& func) -> F
{
    return m_query(std::forward<F>(func));
}

template <ddge::ecs::query_filter_c... Filters_T>
    requires(sizeof...(Filters_T) != 0)
auto ddge::exec::accessors::Query<Filters_T...>::count() -> std::size_t
{
    return m_query.count();
}
