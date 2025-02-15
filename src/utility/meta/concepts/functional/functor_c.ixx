module;

#include <type_traits>

export module utility.meta.concepts.functional.functor_c;

namespace util::meta {

export template <typename F>
concept functor_c = std::is_class_v<F> && requires { &F::operator(); };

}   // namespace util::meta
