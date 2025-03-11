export module utility.meta.type_traits.functional.invoke_result_of;

import utility.meta.concepts.functional.function;
import utility.meta.concepts.functional.functor;
import utility.meta.concepts.functional.member_function_pointer;
import utility.meta.type_traits.functional.signature;

namespace util::meta {

template <typename F>
struct invoke_result_of;

template <function_c F>
struct invoke_result_of<F> {
    using type = typename signature<F>::result_t;
};

template <member_function_pointer_c F>
struct invoke_result_of<F> {
    using type = typename signature<F>::result_t;
};

template <functor_c F>
struct invoke_result_of<F> {
    using type = typename signature<decltype(&F::operator())>::result_t;
};

export template <typename F>
using invoke_result_of_t = typename invoke_result_of<F>::type;

}   // namespace util::meta
