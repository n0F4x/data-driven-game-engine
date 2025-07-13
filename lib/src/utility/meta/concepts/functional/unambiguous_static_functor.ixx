module;

#include <type_traits>

export module utility.meta.concepts.functional.unambiguous_static_functor;

import utility.meta.concepts.functional.unambiguous_explicit_functor;

namespace util::meta {

export template <typename T>
concept unambiguous_static_functor_c =
    std::is_class_v<std::remove_cvref_t<T>>   //
    && requires { &std::remove_cvref_t<T>::operator(); }
    && !std::is_member_function_pointer_v<decltype(&std::remove_cvref_t<T>::operator())>
    && !unambiguous_explicit_functor_c<T>;

}   // namespace util::meta
