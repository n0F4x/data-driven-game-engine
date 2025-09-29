module;

#include <concepts>
#include <type_traits>

export module ddge.modules.execution.raw_task_c;

import ddge.utility.meta.concepts.functional.unambiguously_invocable;
import ddge.utility.meta.concepts.type_list.type_list_all_of;
import ddge.utility.meta.type_traits.functional.arguments_of;

template <typename T>
struct IsConstructibleFromReference
    : std::bool_constant<std::constructible_from<T, std::remove_cvref_t<T>&>> {};

namespace ddge::exec {

export template <typename T>
concept raw_task_c =
    util::meta::unambiguously_invocable_c<T>
    && util::meta::
        type_list_all_of_c<util::meta::arguments_of_t<T>, ::IsConstructibleFromReference>;

}   // namespace ddge::exec
