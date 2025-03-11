module;

#include <type_traits>

export module utility.meta.type_traits.is_specialization_of;

namespace util::meta {

export template <typename, template <typename...> typename>
struct is_specialization_of : std::false_type {};

export template <template <typename...> typename TypeList_T, typename... Ts>
struct is_specialization_of<TypeList_T<Ts...>, TypeList_T> : std::true_type {};

export template <typename T, template <typename...> typename TypeList_T>
constexpr bool is_specialization_of_v = is_specialization_of<T, TypeList_T>::value;

}   // namespace util::meta
