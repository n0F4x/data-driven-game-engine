module;

#include <type_traits>

export module utility.type_traits.type_list.is_type_list;

namespace util::meta {

export template <typename>
struct is_type_list : std::false_type {};

template <template <typename...> typename TypeList_T, typename... Ts>
struct is_type_list<TypeList_T<Ts...>> : std::true_type {};

export template <typename T>
constexpr bool is_type_list_v = is_type_list<T>::value;

}   // namespace util::meta
