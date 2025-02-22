module;

#include <tuple>

export module utility.tuple.tuple_drop_front;

import utility.meta.type_traits.integer_sequence.offset_integer_sequence;
import utility.tuple.tuple_select;

namespace util {

export template <typename Tuple_T>
auto tuple_drop_front(Tuple_T&& tuple)
{
    constexpr static size_t size{ std::tuple_size_v<Tuple_T> };
    using Indices = meta::offset_integer_sequence_t<std::make_index_sequence<size - 1>, 1>;
    return tuple_select(std::forward<Tuple_T>(tuple), Indices{});
}

}   // namespace util
