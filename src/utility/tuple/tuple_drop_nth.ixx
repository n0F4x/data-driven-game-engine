module;

#include <tuple>

export module utility.tuple.tuple_drop_nth;

import utility.meta.type_traits.integer_sequence.concat_integer_sequence;
import utility.meta.type_traits.integer_sequence.offset_integer_sequence;
import utility.tuple.tuple_select;

namespace util {

export template <size_t N, typename Tuple_T>
auto tuple_drop_nth(Tuple_T&& tuple)
{
    constexpr static size_t size{ std::tuple_size_v<Tuple_T> };
    using First = std::make_index_sequence<N>;
    using Rest =
        meta::offset_integer_sequence_t<std::make_index_sequence<size - N - 1>, N + 1>;
    using Indices = meta::concat_integer_sequence_t<First, Rest>;
    return tuple_select(std::forward<Tuple_T>(tuple), Indices{});
}

}   // namespace util
