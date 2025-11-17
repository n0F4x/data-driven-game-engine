module;

#include <type_traits>

export module ddge.utility.meta.type_traits.functional.signature_of;

import ddge.utility.meta.concepts.functional.function;
import ddge.utility.meta.concepts.functional.function_pointer;
import ddge.utility.meta.concepts.functional.function_reference;
import ddge.utility.meta.concepts.functional.member_function_pointer;
import ddge.utility.meta.concepts.functional.unambiguous_functor;
import ddge.utility.meta.concepts.functional.unambiguously_invocable;
import ddge.utility.meta.type_traits.functional.Signature;

namespace ddge::util::meta {

template <unambiguously_invocable_c F>
struct signature_of;

template <function_c F>
struct signature_of<F> {
    using type = Signature<F>::type;
};

template <typename F>
    requires function_pointer_c<std::remove_reference_t<F>>
struct signature_of<F> {
    using type = Signature<std::remove_pointer_t<std::remove_reference_t<F>>>::type;
};

template <function_reference_c F>
struct signature_of<F> {
    using type = Signature<std::remove_reference_t<F>>::type;
};

template <member_function_pointer_c F>
struct signature_of<F> {
    using type = Signature<F>::type;
};

template <unambiguous_functor_c F>
struct signature_of<F> {
    using type =
        typename signature_of<decltype(&std::remove_cvref_t<F>::operator())>::type;
};

export template <typename F>
using signature_of_t = typename signature_of<F>::type;

}   // namespace ddge::util::meta
