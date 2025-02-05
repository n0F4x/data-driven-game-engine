module;

#include <utility>

// reference: https://devblogs.microsoft.com/oldnewthing/20200625-00/?p=103903

export module utility.tuple;

import utility.concepts.integer_sequence_c;
import utility.concepts.type_list.type_list_c;
import utility.type_traits.integer_sequence.concat_integer_sequence;
import utility.type_traits.integer_sequence.integer_sequence_size;
import utility.type_traits.integer_sequence.offset_integer_sequence;
import utility.type_traits.type_list.type_list_size;
import utility.concepts.tuple_like_c;

namespace util {

export template <typename Tuple_T, size_t... Ints_V>
auto select_tuple(Tuple_T&& tuple, std::index_sequence<Ints_V...>)
{
    return std::tuple<std::tuple_element_t<Ints_V, Tuple_T>...>{
        std::get<Ints_V>(std::forward<Tuple_T>(tuple))...
    };
}

export template <typename Tuple_T>
auto remove_first(Tuple_T&& tuple)
{
    constexpr static size_t size{ std::tuple_size_v<Tuple_T> };
    using Indices = meta::offset_integer_sequence_t<std::make_index_sequence<size - 1>, 1>;
    return select_tuple(std::forward<Tuple_T>(tuple), Indices{});
}

export template <typename Tuple_T>
auto remove_last(Tuple_T&& tuple)
{
    constexpr static size_t size{ std::tuple_size_v<Tuple_T> };
    using Indices = std::make_index_sequence<size - 1>;
    return select_tuple(std::forward<Tuple_T>(tuple), Indices{});
}

export template <size_t N, typename Tuple_T>
auto remove_nth(Tuple_T&& tuple)
{
    constexpr static size_t size{ std::tuple_size_v<Tuple_T> };
    using First = std::make_index_sequence<N>;
    using Rest =
        meta::offset_integer_sequence_t<std::make_index_sequence<size - N - 1>, N + 1>;
    using Indices = meta::concat_integer_sequence_t<First, Rest>;
    return select_tuple(std::forward<Tuple_T>(tuple), Indices{});
}

export template <template <typename...> typename Tuple_T, typename... Ts>
auto make_tuple(Ts&&... args) -> Tuple_T<Ts...>
{
    return { std::forward<Ts>(args)... };
}

template <typename, template <typename...> typename, typename>
struct generate_tuple_from_helper;

template <
    template <typename...> typename TypeList_T,
    typename... Ts,
    template <typename...> typename Tuple_T,
    size_t... Indices_V>
struct generate_tuple_from_helper<
    TypeList_T<Ts...>,
    Tuple_T,
    std::index_sequence<Indices_V...>> {
    template <typename Generator_T>
    static auto operator()(Generator_T&& generator)
    {
        return make_tuple<Tuple_T>(generator.template operator()<Ts...[Indices_V]>()...);
    }
};

export template <
    meta::type_list_c TypeList_T,
    template <typename...> typename Tuple_T = std::tuple,
    typename Generator_T>
auto generate_tuple_from(Generator_T&& generator)
{
    constexpr static size_t size{ meta::type_list_size_v<TypeList_T> };

    if constexpr (size == 0) {
        return Tuple_T<>{};
    }

    return generate_tuple_from_helper<TypeList_T, Tuple_T, std::make_index_sequence<size>>::
        operator()(std::forward<Generator_T>(generator));
}

template <
    template <typename, size_t...> typename IntegerSequence_T,
    typename Integer_T,
    size_t... Integers_T,
    template <typename...> typename Tuple_T,
    size_t... Indices_V>
struct generate_tuple_from_helper<
    IntegerSequence_T<Integer_T, Integers_T...>,
    Tuple_T,
    std::index_sequence<Indices_V...>> {
    template <typename Generator_T>
    static auto operator()(Generator_T&& generator)
    {
        return make_tuple<Tuple_T>(generator.template operator(
        )<Integers_T...[Indices_V]>()...);
    }
};

export template <
    meta::integer_sequence_c IntegerSequence_T,
    template <typename...> typename Tuple_T = std::tuple,
    typename Generator_T>
auto generate_tuple_from(Generator_T&& generator)
{
    constexpr static size_t size{ meta::integer_sequence_size_v<IntegerSequence_T> };

    if constexpr (size == 0) {
        return Tuple_T<>{};
    }

    return generate_tuple_from_helper<
        IntegerSequence_T,
        Tuple_T,
        std::make_index_sequence<size>>::operator()(std::forward<Generator_T>(generator));
}

}   // namespace util
