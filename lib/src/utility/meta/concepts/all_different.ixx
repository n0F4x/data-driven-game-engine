export module ddge.utility.meta.concepts.all_different;

import ddge.utility.meta.type_traits.type_list.type_list_unique;
import ddge.utility.meta.type_traits.type_list.type_list_size;

template <typename...>
struct DummyTypeList {};

namespace ddge::util::meta {

export template <typename... Ts>
concept all_different_c = type_list_size_v<type_list_unique_t<DummyTypeList<Ts...>>>
                       == sizeof...(Ts);

}   // namespace ddge::util::meta
