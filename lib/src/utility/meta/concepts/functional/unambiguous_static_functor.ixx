module;

#include <type_traits>

export module ddge.utility.meta.concepts.functional.unambiguous_static_functor;

import ddge.utility.meta.concepts.functional.unambiguous_explicit_functor;

namespace ddge::util::meta {

export template <typename T>
concept unambiguous_static_functor_c =
    std::is_class_v<std::remove_cvref_t<T>>   //
    && requires { &std::remove_cvref_t<T>::operator(); }
    && !std::is_member_function_pointer_v<decltype(&std::remove_cvref_t<T>::operator())>
    && !unambiguous_explicit_functor_c<T>;

}   // namespace ddge::util::meta
