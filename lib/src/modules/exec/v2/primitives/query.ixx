module;

#include <functional>
#include <type_traits>
#include <utility>

export module ddge.modules.exec.v2.primitives.query;

import ddge.modules.ecs;

import ddge.modules.exec.accessors.ecs;
import ddge.modules.exec.v2.Cardinality;
import ddge.modules.exec.v2.ExecPolicy;
import ddge.modules.exec.v2.primitives.as_task;
import ddge.modules.exec.v2.TaskBlueprint;

import ddge.utility.meta.type_traits.functional.arguments_of;
import ddge.utility.meta.type_traits.type_list.type_list_contains;
import ddge.utility.meta.type_traits.type_list.type_list_index_of;
import ddge.utility.meta.type_traits.type_list.type_list_to;
import ddge.utility.meta.type_traits.type_list.type_list_transform;
import ddge.utility.meta.algorithms.apply;
import ddge.utility.TypeList;

namespace ddge::exec::v2 {

export template <
    ExecPolicy                      execution_policy_T = ExecPolicy::eDefault,
    ecs::deducable_query_function_c F>
[[nodiscard]]
auto query(F&& func) -> TaskBlueprint<void, Cardinality::eSingle>
{
    using Query = util::meta::type_list_to_t<
        util::meta::
            type_list_transform_t<util::meta::arguments_of_t<F>, std::remove_reference>,
        accessors::Query>;

    return as_task<execution_policy_T>(
        [x_func = std::forward<F>(func)](Query& entities) mutable -> void {
            util::meta::apply<util::meta::arguments_of_t<F>>(   //
                [&entities, &x_func]<typename... QueryFilters_T> {
                    return entities.for_each(                   //
                        [&x_func]<typename... Args_T>(Args_T&&... args) {
                            using DecayedArgsTypeList =
                                util::TypeList<std::decay_t<Args_T>...>;
                            std::invoke(
                                x_func,
                                [&args...]<
                                    typename QueryFilter_T>(std::type_identity<QueryFilter_T>)
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
        }
    );
}

}   // namespace ddge::exec::v2
