module;

#include <functional>
#include <type_traits>
#include <utility>

export module ddge.modules.ecs:query.query;

import ddge.utility.meta.concepts.functional.unambiguously_invocable;
import ddge.utility.meta.concepts.type_list.type_list_none_of;
import ddge.utility.meta.type_traits.functional.arguments_of;
import ddge.utility.meta.type_traits.type_list.type_list_contains;
import ddge.utility.meta.type_traits.type_list.type_list_index_of;
import ddge.utility.meta.type_traits.type_list.type_list_to;
import ddge.utility.meta.type_traits.type_list.type_list_transform;
import ddge.utility.TypeList;

import :query.QueryClosure;
import :Registry;

namespace ddge::ecs {

export template <typename F>
concept deducable_query_function_c =
    util::meta::unambiguously_invocable_c<F>
    && util::meta::
        type_list_none_of_c<util::meta::arguments_of_t<F>, std::is_rvalue_reference>
    && requires(ecs::Registry& registry) {
           util::meta::type_list_to_t<
               util::meta::
                   type_list_transform_t<util::meta::arguments_of_t<F>, std::remove_reference>,
               ecs::Query>{ registry };
       };

/**
 * Do not directly alter the registry while querying!
 */
export template <query_filter_c... Filters_T, typename F>
    requires(sizeof...(Filters_T) != 0)
auto query(Registry& registry, F&& func) -> F;

/**
 * Do not directly alter the registry while querying!
 */
export template <deducable_query_function_c F>
auto query(Registry& registry, F&& func) -> F;

export template <query_filter_c... Filters_T>
    requires(sizeof...(Filters_T) != 0)
auto count(Registry& registry) -> std::size_t;

}   // namespace ddge::ecs

template <ddge::ecs::query_filter_c... Filters_T, typename F>
    requires(sizeof...(Filters_T) != 0)
auto ddge::ecs::query(Registry& registry, F&& func) -> F
{
    return Query<Filters_T...>{ registry }(std::forward<F>(func));
}

template <ddge::ecs::deducable_query_function_c F>
auto ddge::ecs::query(Registry& registry, F&& func) -> F
{
    util::meta::apply<util::meta::arguments_of_t<F>>(   //
        [&registry, &func]<typename... QueryFilters_T> {
            return query<std::remove_reference_t<QueryFilters_T>...>(
                registry,
                [&func]<typename... Args_T>(Args_T&&... args) {
                    using DecayedArgsTypeList = util::TypeList<std::decay_t<Args_T>...>;
                    std::invoke(
                        std::forward<F>(func),
                        [&args...]<typename QueryFilter_T>(std::type_identity<QueryFilter_T>)
                            -> decltype(auto) {
                            using DecayedQueryFilter = std::decay_t<QueryFilter_T>;
                            if constexpr (util::meta::type_list_contains_v<
                                              DecayedArgsTypeList,
                                              DecayedQueryFilter>)
                            {
                                constexpr std::size_t argument_index{
                                    util::meta::type_list_index_of_v<
                                        DecayedArgsTypeList,
                                        DecayedQueryFilter>
                                };
                                return std::forward<Args_T...[argument_index]>(
                                    args...[argument_index]
                                );
                            }
                            else {
                                return QueryFilter_T{};
                            }
                        }(std::type_identity<QueryFilters_T>{})...
                    );
                }
            );
        }
    );
    return std::forward<F>(func);
}

template <ddge::ecs::query_filter_c... Filters_T>
    requires(sizeof...(Filters_T) != 0)
auto ddge::ecs::count(Registry& registry) -> std::size_t
{
    return Query<Filters_T...>{ registry }.count();
}
