export module utility.type_traits.functional.invoke_result_of;

import utility.concepts.functional.function_c;
import utility.concepts.functional.functor_c;
import utility.concepts.functional.member_function_pointer_c;
import utility.type_traits.functional.signature;

namespace util::meta {
// NOLINTBEGIN(readability-identifier-naming)
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

// NOLINTEND(readability-identifier-naming)
}   // namespace util::meta
