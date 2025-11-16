module;

#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>

export module ddge.utility.not_fn;

import ddge.utility.meta.concepts.functional.unambiguously_invocable;
import ddge.utility.meta.type_traits.functional.result_of;
import ddge.utility.containers.FunctionWrapper;

template <ddge::util::meta::unambiguously_invocable_c Invocable_T>
    requires std::same_as<ddge::util::meta::result_of_t<Invocable_T>, bool>
using NotFn = ddge::util::BasicFunctionWrapper<Invocable_T, std::logical_not{}>;

namespace ddge::util {

export template <typename F>
    requires requires { std::type_identity<::NotFn<std::remove_cvref_t<F>>>{}; }
constexpr auto not_fn(F&& func) -> ::NotFn<std::remove_cvref_t<F>>
{
    return ::NotFn<std::remove_cvref_t<F>>{ std::forward<F>(func) };
}

}   // namespace ddge::util
