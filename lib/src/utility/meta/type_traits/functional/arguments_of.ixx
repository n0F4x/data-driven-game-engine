export module utility.meta.type_traits.functional.arguments_of;

import utility.meta.concepts.functional.function;
import utility.meta.concepts.functional.functor;
import utility.meta.concepts.functional.member_function_pointer;
import utility.meta.type_traits.functional.signature;

namespace util::meta {
// NOLINTBEGIN(readability-identifier-naming)

template <typename F>
struct arguments_of;

template <function_c F>
struct arguments_of<F> {
    using type = typename signature<F>::arguments_t;
};

template <member_function_pointer_c F>
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
}   // namespace util::meta
