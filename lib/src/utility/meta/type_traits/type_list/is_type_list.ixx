module;

#include <type_traits>

export module ddge.utility.meta.type_traits.type_list.is_type_list;

namespace ddge::util::meta {

export template <typename>
struct is_type_list : std::false_type {};

template <template <typename...> typename TypeList_T, typename... Ts>
struct is_type_list<TypeList_T<Ts...>> : std::true_type {};

export template <typename T>
inline constexpr bool is_type_list_v = is_type_list<T>::value;

}   // namespace ddge::util::meta
