module;

#include <type_traits>

export module ddge.util.meta.type_traits.functional.result_of;

import ddge.util.meta.concepts.functional.function;
import ddge.util.meta.concepts.functional.function_pointer;
import ddge.util.meta.concepts.functional.function_reference;
import ddge.util.meta.concepts.functional.member_function_pointer;
import ddge.util.meta.concepts.functional.unambiguous_functor;
import ddge.util.meta.concepts.functional.unambiguously_invocable;
import ddge.util.meta.type_traits.functional.Signature;

namespace ddge::util::meta {

template <unambiguously_invocable_c F>
struct result_of;

template <function_c F>
struct result_of<F> {
    using type = typename Signature<F>::result_t;
};

template <typename F>
    requires function_pointer_c<std::remove_reference_t<F>>
struct result_of<F> {
    using type =
        typename Signature<std::remove_pointer_t<std::remove_reference_t<F>>>::result_t;
};

template <function_reference_c F>
struct result_of<F> {
    using type = typename Signature<std::remove_reference_t<F>>::result_t;
};

template <member_function_pointer_c F>
struct result_of<F> {
    using type = typename Signature<F>::result_t;
};

template <unambiguous_functor_c F>
struct result_of<F> {
    using type = typename result_of<decltype(&std::remove_cvref_t<F>::operator())>::type;
};

export template <typename F>
using result_of_t = typename result_of<F>::type;

}   // namespace ddge::util::meta
