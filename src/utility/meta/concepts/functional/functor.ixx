module;

#include <type_traits>

export module utility.meta.concepts.functional.functor;

namespace util::meta {

export template <typename F>
concept functor_c = std::is_class_v<F> && requires { &F::operator(); };

}   // namespace util::meta
