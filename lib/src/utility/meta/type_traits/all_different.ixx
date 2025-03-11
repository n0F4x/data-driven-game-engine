export module utility.meta.type_traits.all_different;

import utility.meta.type_traits.type_list.type_list_unique;
import utility.meta.type_traits.type_list.type_list_size;

namespace util::meta {

template <typename...>
struct DummyTypeList {};

export template <typename... Ts>
constexpr bool all_different_v =
    type_list_size_v<type_list_unique_t<DummyTypeList<Ts...>>> == sizeof...(Ts);

}   // namespace util::meta
