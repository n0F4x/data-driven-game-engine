module;

#include <type_traits>

export module utility.meta.type_traits.functional.arguments_of;

import utility.meta.concepts.functional.function;
import utility.meta.concepts.functional.function_pointer;
import utility.meta.concepts.functional.function_reference;
import utility.meta.concepts.functional.member_function_pointer;
import utility.meta.concepts.functional.unambiguous_explicit_functor;
import utility.meta.concepts.functional.unambiguous_implicit_functor;
import utility.meta.concepts.functional.unambiguously_invocable;
import utility.meta.type_traits.functional.signature;
import utility.meta.type_traits.type_list.type_list_drop_front;

namespace util::meta {

template <typename F>
struct arguments_of;

template <function_c F>
struct arguments_of<F> {
    using type = typename signature<F>::arguments_t;
};

template <typename F>
    requires function_pointer_c<std::remove_reference_t<F>>
struct arguments_of<F> {
    using type =
        typename signature<std::remove_pointer_t<std::remove_reference_t<F>>>::arguments_t;
};

template <function_reference_c F>
struct arguments_of<F> {
    using type = typename signature<std::remove_reference_t<F>>::arguments_t;
};

template <member_function_pointer_c F>
struct arguments_of<F> {
    using type = typename signature<F>::arguments_t;
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

export template <typename F>
using arguments_of_t = typename arguments_of<F>::type;

}   // namespace util::meta
