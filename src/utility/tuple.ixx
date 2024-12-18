module;

#include <utility>

// reference: https://devblogs.microsoft.com/oldnewthing/20200625-00/?p=103903

export module utility.tuple;

import utility.meta.concat;
import utility.meta.offset;
import utility.meta.tuple.tuple_like;

namespace utils {

export template <typename Tuple, size_t... Ints>
auto select_tuple(Tuple&& tuple, std::index_sequence<Ints...>)
{
    return std::tuple<std::tuple_element_t<Ints, Tuple>...>{
        std::get<Ints>(std::forward<Tuple>(tuple))...
    };
}

export template <typename Tuple>
auto remove_first(Tuple&& tuple)
{
    constexpr static size_t size{ std::tuple_size_v<Tuple> };
    using Indices = meta::offset_t<1, std::make_index_sequence<size - 1>>;
    return select_tuple(std::forward<Tuple>(tuple), Indices{});
}

export template <typename Tuple>
auto remove_last(Tuple&& tuple)
{
    constexpr static size_t size{ std::tuple_size_v<Tuple> };
    using Indices = std::make_index_sequence<size - 1>;
    return select_tuple(std::forward<Tuple>(tuple), Indices{});
}

export template <size_t N, typename Tuple>
auto remove_nth(Tuple&& tuple)
{
    constexpr static size_t size{ std::tuple_size_v<Tuple> };
    using First   = std::make_index_sequence<N>;
    using Rest    = meta::offset_t<N + 1, std::make_index_sequence<size - N - 1>>;
    using Indices = meta::concat_t<First, Rest>;
    return select_tuple(std::forward<Tuple>(tuple), Indices{});
}

template <typename Tuple, typename Generator, size_t... Ints>
auto generate_tuple(Generator&& generator, std::index_sequence<Ints...>) -> Tuple
{
    return Tuple(generator.template operator()<std::tuple_element_t<Ints, Tuple>>()...);
}

export template <meta::tuple_like Tuple, typename Generator>
auto generate_tuple(Generator&& generator) -> Tuple
{
    constexpr static size_t size{ std::tuple_size_v<Tuple> };

    if constexpr (size == 0) {
        return Tuple{};
    }

    using Indices = std::make_index_sequence<size>;
    return generate_tuple<Tuple>(std::forward<Generator>(generator), Indices{});
}

}   // namespace utils
