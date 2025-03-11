module;

#include <tuple>

export module utility.tuple.tuple_drop_back;

import utility.tuple.tuple_select;

namespace util {

export template <typename Tuple_T>
auto tuple_drop_back(Tuple_T&& tuple)
{
    constexpr static size_t size{ std::tuple_size_v<Tuple_T> };
    using Indices = std::make_index_sequence<size - 1>;
    return tuple_select(std::forward<Tuple_T>(tuple), Indices{});
}

}   // namespace util
