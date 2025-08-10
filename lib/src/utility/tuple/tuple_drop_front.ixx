module;

#include <tuple>

export module ddge.utility.tuple.tuple_drop_front;

import ddge.utility.meta.type_traits.integer_sequence.integer_sequence_offset;
import ddge.utility.tuple.tuple_select;

namespace ddge::util {

export template <typename Tuple_T>
auto tuple_drop_front(Tuple_T&& tuple)
{
    constexpr static std::size_t size{ std::tuple_size_v<Tuple_T> };
    using Indices = meta::integer_sequence_offset_t<std::make_index_sequence<size - 1>, 1>;
    return tuple_select(std::forward<Tuple_T>(tuple), Indices{});
}

}   // namespace ddge::util
