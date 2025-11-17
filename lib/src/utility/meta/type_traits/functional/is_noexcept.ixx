module;

#include <type_traits>

export module ddge.utility.meta.type_traits.functional.is_noexcept;

import ddge.utility.meta.concepts.functional.function;
import ddge.utility.meta.concepts.functional.function_pointer;
import ddge.utility.meta.concepts.functional.function_reference;
import ddge.utility.meta.concepts.functional.member_function_pointer;
import ddge.utility.meta.concepts.functional.unambiguous_functor;
import ddge.utility.meta.concepts.functional.unambiguously_invocable;
import ddge.utility.meta.type_traits.functional.Signature;

namespace ddge::util::meta {

template <unambiguously_invocable_c F>
struct IsNoexcept;

template <function_c F>
struct IsNoexcept<F> {
    constexpr static bool value = Signature<F>::is_noexcept();
};

template <typename F>
    requires function_pointer_c<std::remove_reference_t<F>>
struct IsNoexcept<F> {
    constexpr static bool value =
        Signature<std::remove_pointer_t<std::remove_reference_t<F>>>::is_noexcept();
};

template <function_reference_c F>
struct IsNoexcept<F> {
    constexpr static bool value = Signature<std::remove_reference_t<F>>::is_noexcept();
};

template <member_function_pointer_c F>
struct IsNoexcept<F> {
    constexpr static bool value = Signature<F>::is_noexcept();
};

template <unambiguous_functor_c F>
struct IsNoexcept<F> {
    constexpr static bool value =
        IsNoexcept<decltype(&std::remove_cvref_t<F>::operator())>::value;
};

export template <typename F>
inline constexpr bool is_noexcept_v = IsNoexcept<F>::value;

}   // namespace ddge::util::meta
