export module utility.meta.concepts.all_different;

import utility.meta.type_traits.type_list.type_list_unique;
import utility.meta.type_traits.type_list.type_list_size;

template <typename...>
struct DummyTypeList {};

namespace util::meta {

export template <typename... Ts>
concept all_different_c = type_list_size_v<type_list_unique_t<DummyTypeList<Ts...>>>
                       == sizeof...(Ts);

}   // namespace util::meta
