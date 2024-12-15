export module utility.meta.functional.invoke_result_of;

import utility.meta.functional.signature;
import utility.meta.functional.concepts;

namespace utils::meta {
// NOLINTBEGIN(readability-identifier-naming)
template <typename F>
struct invoke_result_of;

template <function_c F>
struct invoke_result_of<F> {
    using type = typename signature<F>::result_t;
};

template <member_function_c F>
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
}   // namespace utils::meta
