module;

#include <type_traits>

export module ddge.utility.meta.type_traits.functional.arguments_of;

import ddge.utility.meta.concepts.functional.function;
import ddge.utility.meta.concepts.functional.function_pointer;
import ddge.utility.meta.concepts.functional.function_reference;
import ddge.utility.meta.concepts.functional.member_function_pointer;
import ddge.utility.meta.concepts.functional.unambiguous_explicit_functor;
import ddge.utility.meta.concepts.functional.unambiguous_implicit_functor;
import ddge.utility.meta.concepts.functional.unambiguous_static_functor;
import ddge.utility.meta.concepts.functional.unambiguously_invocable;
import ddge.utility.meta.type_traits.functional.Signature;
import ddge.utility.meta.type_traits.type_list.type_list_drop_front;

namespace ddge::util::meta {

template <unambiguously_invocable_c F>
struct arguments_of;

template <function_c F>
struct arguments_of<F> {
    using type = typename Signature<F>::arguments_t;
};

template <typename F>
    requires function_pointer_c<std::remove_reference_t<F>>
struct arguments_of<F> {
    using type =
        typename Signature<std::remove_pointer_t<std::remove_reference_t<F>>>::arguments_t;
};

template <function_reference_c F>
struct arguments_of<F> {
    using type = typename Signature<std::remove_reference_t<F>>::arguments_t;
};

template <member_function_pointer_c F>
struct arguments_of<F> {
    using type = typename Signature<F>::arguments_t;
};

template <unambiguous_explicit_functor_c F>
struct arguments_of<F> {
    using type = type_list_drop_front_t<
        typename arguments_of<decltype(&std::remove_cvref_t<F>::operator())>::type>;
};

template <unambiguous_implicit_functor_c F>
struct arguments_of<F> {
    using type =
        typename arguments_of<decltype(&std::remove_cvref_t<F>::operator())>::type;
};

template <unambiguous_static_functor_c F>
struct arguments_of<F> {
    using type =
        typename arguments_of<decltype(&std::remove_cvref_t<F>::operator())>::type;
};

export template <typename F>
using arguments_of_t = typename arguments_of<F>::type;

}   // namespace ddge::util::meta
