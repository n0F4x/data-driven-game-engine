module;

#include <tuple>

export module utility.tuple.tuple_drop_nth;

import utility.meta.type_traits.integer_sequence.integer_sequence_concat;
import utility.meta.type_traits.integer_sequence.integer_sequence_offset;
import utility.tuple.tuple_select;

namespace util {

export template <size_t N, typename Tuple_T>
auto tuple_drop_nth(Tuple_T&& tuple)
{
    constexpr static size_t size{ std::tuple_size_v<Tuple_T> };
    using First = std::make_index_sequence<N>;
    using Rest =
        meta::integer_sequence_offset_t<std::make_index_sequence<size - N - 1>, N + 1>;
    using Indices = meta::integer_sequence_concat_t<First, Rest>;
    return tuple_select(std::forward<Tuple_T>(tuple), Indices{});
}

}   // namespace util
