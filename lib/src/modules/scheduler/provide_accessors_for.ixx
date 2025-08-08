module;

#include <tuple>
#include <type_traits>

export module modules.scheduler.provide_accessors_for;

import modules.scheduler.accessor_c;
import modules.scheduler.Nexus;

import utility.meta.concepts.functional.unambiguously_invocable;
import utility.meta.type_traits.functional.arguments_of;
import utility.meta.type_traits.type_list.type_list_transform;
import utility.meta.type_traits.type_list.type_list_to;
import utility.TypeList;

namespace modules::scheduler {

template <typename F>
using accessors_tuple_for_t = util::meta::type_list_to_t<
    util::meta::type_list_transform_t<util::meta::arguments_of_t<F>, std::remove_cvref>,
    std::tuple>;

export template <util::meta::unambiguously_invocable_c F>
[[nodiscard]]
auto provide_accessors_for(Nexus& nexus) -> accessors_tuple_for_t<F>;

}   // namespace modules::scheduler

template <util::meta::unambiguously_invocable_c F>
auto modules::scheduler::provide_accessors_for(modules::scheduler::Nexus& nexus)
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
