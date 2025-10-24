module;

#include <cstddef>
#include <type_traits>
#include <utility>

export module ddge.modules.exec.accessors.ecs:Query;

import :locks.ExclusiveComponentLock;
import :locks.SharedComponentLock;
import :locks.SharedRegistryLock;

import ddge.modules.ecs;
import ddge.modules.exec.locks.Lockable;

import ddge.utility.meta.concepts.specialization_of;
import ddge.utility.meta.type_traits.type_list.type_list_filter;
import ddge.utility.meta.type_traits.type_list.type_list_push_front;
import ddge.utility.meta.type_traits.type_list.type_list_to;
import ddge.utility.meta.type_traits.type_list.type_list_transform;
import ddge.utility.TypeList;

namespace ddge::exec::accessors {

inline namespace ecs {

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

// ReSharper disable once CppFunctionIsNotImplemented
template <typename Component_T>
    requires ddge::ecs::queryable_component_c<std::remove_const_t<Component_T>>
auto to_component_t() -> Component_T;

// ReSharper disable once CppFunctionIsNotImplemented
template <ddge::util::meta::specialization_of_c<ddge::ecs::Optional> Optional_T>
auto to_component_t() -> typename Optional_T::ValueType;

template <typename ComponentOrOptional>
struct LockOf {
    using Component = decltype(to_component_t<ComponentOrOptional>());

    using type = std::conditional_t<
        std::is_const_v<Component>,
        SharedComponentLock<std::remove_const_t<Component>>,
        ExclusiveComponentLock<std::remove_const_t<Component>>>;
};

template <typename... Filters_T>
using CustomizedLockable = util::meta::type_list_to_t<
    util::meta::type_list_push_front_t<
        util::meta::type_list_transform_t<
            util::meta::
                type_list_filter_t<util::TypeList<Filters_T...>, IsQueryableComponentOrOptional>,
            LockOf>,
        SharedRegistryLock>,
    Lockable>;

export template <ddge::ecs::query_filter_c... Filters_T>
    requires(sizeof...(Filters_T) != 0)
class Query : public CustomizedLockable<Filters_T...> {
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
