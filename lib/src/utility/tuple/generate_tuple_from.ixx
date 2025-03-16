module;

#include <utility>

export module utility.tuple.generate_tuple_from;

import utility.meta.concepts.integer_sequence.integer_sequence;
import utility.meta.concepts.type_list;
import utility.meta.type_traits.integer_sequence.integer_sequence_size;
import utility.meta.type_traits.type_list.type_list_size;

namespace util {

template <template <typename...> typename TypeList_T, typename... Ts>
auto make_tuple(Ts&&... args) -> TypeList_T<Ts...>
{
    return { std::forward<Ts>(args)... };
}

template <typename, template <typename...> typename, typename>
struct generate_tuple_from_helper;

template <
    template <typename...> typename TypeList_T,
    typename... Ts,
    template <typename...> typename Tuple_T,
    size_t... indices_T>
struct generate_tuple_from_helper<
    TypeList_T<Ts...>,
    Tuple_T,
    std::index_sequence<indices_T...>> {
    template <typename Generator_T>
    static auto operator()(Generator_T&& generator)
    {
        return make_tuple<Tuple_T>(generator.template operator()<Ts...[indices_T]>()...);
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
    template <typename T, T...> typename IntegerSequence_T,
    typename Integer_T,
    Integer_T... integers_T,
    template <typename...> typename Tuple_T,
    size_t... indices_T>
struct generate_tuple_from_helper<
    IntegerSequence_T<Integer_T, integers_T...>,
    Tuple_T,
    std::index_sequence<indices_T...>> {
    template <typename Generator_T>
    static auto operator()(Generator_T&& generator)
    {
        return make_tuple<Tuple_T>(generator.template operator(
        )<integers_T...[indices_T]>()...);
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
