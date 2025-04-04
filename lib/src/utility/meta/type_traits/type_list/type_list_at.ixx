module;

#include <cstdint>

export module utility.meta.type_traits.type_list.type_list_at;

namespace util::meta {

export template <typename TypeList_T, size_t index_T>
struct type_list_at;

template <template <typename...> typename TypeList_T, typename... Ts, size_t index_T>
struct type_list_at<TypeList_T<Ts...>, index_T> {
    using type = Ts...[index_T];
};

export template <typename TypeList_T, size_t index_T>
using type_list_at_t = typename type_list_at<TypeList_T, index_T>::type;

}   // namespace util::meta
