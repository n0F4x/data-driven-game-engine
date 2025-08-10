module;

#include <type_traits>

export module ddge.utility.meta.concepts.functional.unambiguous_implicit_functor;

namespace ddge::util::meta {

export template <typename T>
concept unambiguous_implicit_functor_c =
    std::is_class_v<std::remove_cvref_t<T>>   //
    && requires { &std::remove_cvref_t<T>::operator(); }
    && std::is_member_function_pointer_v<decltype(&std::remove_cvref_t<T>::operator())>;

}   // namespace ddge::util::meta
