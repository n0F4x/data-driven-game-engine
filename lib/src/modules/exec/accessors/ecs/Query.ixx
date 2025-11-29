module;

#include <cstddef>
#include <type_traits>
#include <utility>

export module ddge.modules.exec.accessors.ecs.Query;

import ddge.modules.ecs;
import ddge.modules.exec.accessors.ecs.Component;
import ddge.modules.exec.accessors.ecs.Registry;
import ddge.modules.exec.locks.CriticalSectionType;
import ddge.modules.exec.locks.Lock;
import ddge.modules.exec.locks.LockGroup;

import ddge.utility.meta.algorithms.for_each;
import ddge.utility.meta.concepts.specialization_of;
import ddge.utility.meta.type_traits.type_list.type_list_filter;
import ddge.utility.meta.type_traits.type_list.type_list_push_front;
import ddge.utility.meta.type_traits.type_list.type_list_to;
import ddge.utility.meta.type_traits.type_list.type_list_transform;
import ddge.utility.TypeList;

namespace ddge::exec::accessors {

inline namespace ecs {

export template <ddge::ecs::query_filter_c... Filters_T>
    requires(sizeof...(Filters_T) != 0)
class Query {
public:
    constexpr static auto lock_group() -> LockGroup;

    explicit Query(ddge::ecs::Registry& registry);
    Query(const Query&) = delete ("Queries should be taken by reference");
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

namespace {

template <typename Filter_T>
struct IsQueryableComponentOrOptional {
    constexpr static bool value =
        ddge::ecs::queryable_component_c<std::remove_const_t<Filter_T>>
        || (ddge::util::meta::specialization_of_c<Filter_T, ddge::ecs::Optional>
            && requires {
                   requires ddge::ecs::queryable_component_c<
                       std::remove_const_t<typename Filter_T::ValueType>>;
               });
};

template <typename ComponentOrOptional>
struct ToComponent {
    // ReSharper disable once CppFunctionIsNotImplemented
    template <typename Component_T>
        requires ddge::ecs::queryable_component_c<std::remove_const_t<Component_T>>
    static auto to_component_t() -> Component_T;

    // ReSharper disable once CppFunctionIsNotImplemented
    template <ddge::util::meta::specialization_of_c<ddge::ecs::Optional> Optional_T>
    static auto to_component_t() -> typename Optional_T::ValueType;

    using type = decltype(to_component_t<ComponentOrOptional>());
};

}   // namespace

template <ddge::ecs::query_filter_c... Filters_T>
    requires(sizeof...(Filters_T) != 0)
constexpr auto ddge::exec::accessors::ecs::Query<Filters_T...>::lock_group() -> LockGroup
{
    using Components = util::meta::type_list_transform_t<
        util::meta::
            type_list_filter_t<util::TypeList<Filters_T...>, IsQueryableComponentOrOptional>,
        ToComponent>;

    LockGroup lock_group;
    util::meta::for_each<Components>([&lock_group]<typename Component_T> -> void {
        if constexpr (std::is_const_v<Component_T>) {
            lock_group.expand<std::remove_const_t<Component_T>>(Lock{
                CriticalSectionType::eShared });
        }
        else {
            lock_group.expand<Component_T>(Lock{ CriticalSectionType::eExclusive });
        }
    });
    lock_group.expand<Registry>(Lock{ CriticalSectionType::eShared });
    return lock_group;
}

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
