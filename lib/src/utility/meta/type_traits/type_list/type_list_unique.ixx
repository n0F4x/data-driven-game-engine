module;

#include <type_traits>

export module ddge.utility.meta.type_traits.type_list.type_list_unique;

import ddge.utility.meta.type_traits.type_list.type_list_contains;
import ddge.utility.meta.type_traits.type_list.type_list_drop_back;
import ddge.utility.meta.type_traits.type_list.type_list_push_back;

namespace ddge::util::meta {

// TODO: Clang 22 - __builtin_dedup_pack

export template <typename TypeList_T>
struct type_list_unique;

template <template <typename...> typename TypeList_T>
struct type_list_unique<TypeList_T<>> {
    using type = TypeList_T<>;
};

template <template <typename...> typename TypeList_T, typename... Ts>
    requires(sizeof...(Ts) != 0)
struct type_list_unique<TypeList_T<Ts...>>
    : std::conditional<
          type_list_contains_v<
              type_list_drop_back_t<TypeList_T<Ts...>>,
              Ts...[sizeof...(Ts) - 1]>,
          typename type_list_unique<type_list_drop_back_t<TypeList_T<Ts...>>>::type,
          type_list_push_back_t<
              typename type_list_unique<type_list_drop_back_t<TypeList_T<Ts...>>>::type,
              Ts... [sizeof...(Ts) - 1]>> {};

export template <typename TypeList_T>
using type_list_unique_t = typename type_list_unique<TypeList_T>::type;

}   // namespace ddge::util::meta
