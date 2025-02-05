module;

#include <cstdint>

export module utility.type_traits.type_list.type_list_at;

namespace util::meta {

export template <typename TypeList_T, size_t Index_V>
struct type_list_at;

template <template <typename> typename TypeList_T, typename... Ts, size_t Index_V>
struct type_list_at<TypeList_T<Ts...>, Index_V> {
    using type = Ts...[Index_V];
};

export template <typename TypeList_T, size_t Index_V>
using type_list_at_t = typename type_list_at<TypeList_T, Index_V>::type;

}   // namespace util::meta
