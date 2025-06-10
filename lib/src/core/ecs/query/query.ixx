module;

#include <functional>
#include <type_traits>
#include <utility>

export module core.ecs:query.query;

import utility.meta.concepts.functional.unambiguously_invocable;
import utility.meta.concepts.type_list.type_list_none_of;
import utility.meta.type_traits.functional.arguments_of;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.meta.type_traits.type_list.type_list_index_of;
import utility.meta.type_traits.type_list.type_list_to;
import utility.meta.type_traits.type_list.type_list_transform;

import :query.QueryClosure;
import :Registry;

template <typename F>
concept deducable_query_function_c =
    util::meta::unambiguously_invocable_c<F>
    && util::meta::
        type_list_none_of_c<util::meta::arguments_of_t<F>, std::is_rvalue_reference>
    && requires {
           util::meta::type_list_to_t<
               util::meta::
                   type_list_transform_t<util::meta::arguments_of_t<F>, std::remove_reference>,
               core::ecs::QueryClosure>{};
       };

namespace core::ecs {

/**
 * Do not directly alter the registry while querying!
 */
export template <query_parameter_c... Parameters_T, typename F>
    requires(sizeof...(Parameters_T) != 0)
auto query(Registry& registry, F&& func) -> F;

/**
 * Do not directly alter the registry while querying!
 */
export template <deducable_query_function_c F>
auto query(Registry& registry, F&& func) -> F;

}   // namespace core::ecs

template <core::ecs::query_parameter_c... Parameters_T, typename F>
    requires(sizeof...(Parameters_T) != 0)
auto core::ecs::query(Registry& registry, F&& func) -> F
{
    constexpr static QueryClosure<Parameters_T...> query_closure;
    return query_closure(registry, std::forward<F>(func));
}

template <deducable_query_function_c F>
auto core::ecs::query(Registry& registry, F&& func) -> F
{
    util::meta::apply<util::meta::arguments_of_t<F>>(
        [&registry, &func]<typename... QueryParameters_T> {
            return query<std::remove_reference_t<QueryParameters_T>...>(
                registry,
                [&func]<typename... Args_T>(Args_T&&... args) {
                    using DecayedArgsTypeList = std::tuple<std::decay_t<Args_T>...>;
                    std::invoke(
                        std::forward<F>(func),
                        [&args...]<
                            typename QueryParameter_T>(std::type_identity<QueryParameter_T>)
                            -> decltype(auto) {
                            using DecayedQueryParameter = std::decay_t<QueryParameter_T>;
                            if constexpr (util::meta::type_list_contains_v<
                                              DecayedArgsTypeList,
                                              DecayedQueryParameter>)
                            {
                                constexpr size_t argument_index{
                                    util::meta::type_list_index_of_v<
                                        DecayedArgsTypeList,
                                        DecayedQueryParameter>
                                };
                                return std::forward<Args_T...[argument_index]>(
                                    args...[argument_index]
                                );
                            }
                            else {
                                return QueryParameter_T{};
                            }
                        }(std::type_identity<QueryParameters_T>{})...
                    );
                }
            );
        }
    );
    return std::forward<F>(func);
}
