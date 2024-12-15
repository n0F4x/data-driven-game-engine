export module utility.meta.functional.arguments_of;

import utility.meta.functional.signature;
import utility.meta.functional.concepts;

namespace utils::meta {
// NOLINTBEGIN(readability-identifier-naming)

template <typename F>
struct arguments_of;

template <function_c F>
struct arguments_of<F> {
    using type = typename signature<F>::arguments_t;
};

template <member_function_c F>
struct arguments_of<F> {
    using type = typename signature<F>::arguments_t;
};

template <functor_c F>
struct arguments_of<F> {
    using type = typename signature<decltype(&F::operator())>::arguments_t;
};

export template <typename F>
using arguments_of_t = typename arguments_of<F>::type;

// NOLINTEND(readability-identifier-naming)
}   // namespace utils::meta
