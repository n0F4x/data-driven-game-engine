module;

#include <tuple>
#include <type_traits>

export module ddge.modules.execution.provide_accessors_for;

import ddge.modules.execution.accessor_c;
import ddge.modules.execution.Nexus;

import ddge.utility.meta.concepts.functional.unambiguously_invocable;
import ddge.utility.meta.type_traits.functional.arguments_of;
import ddge.utility.meta.type_traits.type_list.type_list_transform;
import ddge.utility.meta.type_traits.type_list.type_list_to;
import ddge.utility.TypeList;

namespace ddge::exec {

template <typename F>
using accessors_tuple_for_t = util::meta::type_list_to_t<
    util::meta::type_list_transform_t<util::meta::arguments_of_t<F>, std::remove_cvref>,
    std::tuple>;

export template <util::meta::unambiguously_invocable_c F>
[[nodiscard]]
auto provide_accessors_for(Nexus& nexus) -> accessors_tuple_for_t<F>;

}   // namespace ddge::exec

template <ddge::util::meta::unambiguously_invocable_c F>
auto ddge::exec::provide_accessors_for(ddge::exec::Nexus& nexus)
    -> accessors_tuple_for_t<F>
{
    return
        [&nexus]<typename... Accessors_T>(util::TypeList<Accessors_T...>)
            -> accessors_tuple_for_t<F> {
            static_assert((accessor_c<Accessors_T> && ...));
            return accessors_tuple_for_t<F>{ nexus.provide<Accessors_T>()... };
        }   //
        (util::meta::
             type_list_transform_t<util::meta::arguments_of_t<F>, std::remove_cvref>{});
}
