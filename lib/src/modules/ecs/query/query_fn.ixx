module;

#include <functional>
#include <type_traits>
#include <utility>

export module modules.ecs:query.query;

import utility.meta.concepts.functional.unambiguously_invocable;
import utility.meta.concepts.type_list.type_list_none_of;
import utility.meta.type_traits.functional.arguments_of;
import utility.meta.type_traits.type_list.type_list_contains;
import utility.meta.type_traits.type_list.type_list_index_of;
import utility.meta.type_traits.type_list.type_list_to;
import utility.meta.type_traits.type_list.type_list_transform;
import utility.TypeList;

import :query.QueryClosure;
import :Registry;

template <typename F>
concept deducable_query_function_c =
    util::meta::unambiguously_invocable_c<F>
    && util::meta::
        type_list_none_of_c<util::meta::arguments_of_t<F>, std::is_rvalue_reference>
    && requires(modules::ecs::Registry& registry) {
           util::meta::type_list_to_t<
               util::meta::
                   type_list_transform_t<util::meta::arguments_of_t<F>, std::remove_reference>,
               modules::ecs::Query>{ registry };
       };

namespace modules::ecs {

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

export template <query_parameter_c... Parameters_T>
    requires(sizeof...(Parameters_T) != 0)
auto count(Registry& registry) -> std::size_t;

}   // namespace modules::ecs

template <modules::ecs::query_parameter_c... Parameters_T, typename F>
    requires(sizeof...(Parameters_T) != 0)
auto modules::ecs::query(Registry& registry, F&& func) -> F
{
    return Query<Parameters_T...>{ registry }(std::forward<F>(func));
}

template <deducable_query_function_c F>
auto modules::ecs::query(Registry& registry, F&& func) -> F
{
    util::meta::apply<util::meta::arguments_of_t<F>>(
        [&registry, &func]<typename... QueryParameters_T> {
            return query<std::remove_reference_t<QueryParameters_T>...>(
                registry,
                [&func]<typename... Args_T>(Args_T&&... args) {
                    using DecayedArgsTypeList = util::TypeList<std::decay_t<Args_T>...>;
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
                                constexpr std::size_t argument_index{
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

template <modules::ecs::query_parameter_c... Parameters_T>
    requires(sizeof...(Parameters_T) != 0)
auto modules::ecs::count(Registry& registry) -> std::size_t
{
    return Query<Parameters_T...>{ registry }.count();
}
