module;

#include <cstddef>

export module utility.meta.type_traits.type_list.type_list_size;

namespace util::meta {

export template <typename TypeList_T>
struct type_list_size;

template <template <typename...> typename TypeList_T, typename... Ts>
struct type_list_size<TypeList_T<Ts...>> {
    constexpr static std::size_t value = sizeof...(Ts);
};

export template <typename TypeList_T>
inline constexpr std::size_t type_list_size_v = type_list_size<TypeList_T>::value;

}   // namespace util::meta
