module;

#include <type_traits>

export module utility.meta.functional.concepts;

namespace utils::meta {
// NOLINTBEGIN(readability-identifier-naming)

export template <typename F>
concept function_c = std::is_function_v<F>;

export template <typename F>
concept member_function_c = std::is_member_function_pointer_v<F>;

export template <typename F>
concept functor_c = std::is_class_v<F> && requires { &F::operator(); };

export template <typename Callable>
concept callable_c = function_c<Callable> || member_function_c<Callable> || functor_c<Callable>;

// NOLINTEND(readability-identifier-naming)
}   // namespace utils::meta
