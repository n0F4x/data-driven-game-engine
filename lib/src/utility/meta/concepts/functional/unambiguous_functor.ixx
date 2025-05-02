module;

#include <type_traits>

export module utility.meta.concepts.functional.unambiguous_functor;

namespace util::meta {

export template <typename T>
concept unambiguous_functor_c = std::is_class_v<std::remove_cvref_t<T>>
                             && requires { &std::remove_cvref_t<T>::operator(); };

}   // namespace util::meta
