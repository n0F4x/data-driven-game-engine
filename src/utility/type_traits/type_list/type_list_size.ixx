module;

#include <cstdint>

export module utility.type_traits.type_list.type_list_size;

namespace util::meta {

export template <typename TypeList_T>
struct type_list_size;

template <template <typename> typename TypeList_T, typename... Ts>
struct type_list_size<TypeList_T<Ts...>> {
    constexpr static size_t value = sizeof...(Ts);
};

export template <typename TypeList_T>
constexpr size_t type_list_size_v = type_list_size<TypeList_T>::value;

}   // namespace util::meta
