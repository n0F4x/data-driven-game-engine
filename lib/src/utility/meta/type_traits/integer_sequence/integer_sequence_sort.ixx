module;

#include <algorithm>
#include <array>

export module ddge.utility.meta.type_traits.integer_sequence.integer_sequence_sort;

import ddge.utility.meta.concepts.integer_sequence.integer_sequence;
import ddge.utility.meta.type_traits.integer_sequence.integer_sequence_size;

template <typename IntegerSequence_T, typename IndexSequence_T>
struct integer_sequence_sort_impl;

template <
    template <typename T_, T_...> typename IntegerSequence_T,
    typename Integer_T,
    Integer_T... integers_T,
    std::size_t... indices_T>
struct integer_sequence_sort_impl<
    IntegerSequence_T<Integer_T, integers_T...>,
    std::index_sequence<indices_T...>> {
    constexpr static std::array sorted_array =   //
        [] -> std::array<Integer_T, sizeof...(integers_T)>
    {
        std::array<Integer_T, sizeof...(integers_T)> result{ integers_T... };
        std::ranges::sort(result);
        return result;
    }
    ();

    using type = IntegerSequence_T<Integer_T, sorted_array[indices_T]...>;
};

namespace ddge::util::meta {

export template <integer_sequence_c IntegerSequence_T>
struct integer_sequence_sort {
    using type = typename integer_sequence_sort_impl<
        IntegerSequence_T,
        std::make_index_sequence<integer_sequence_size_v<IntegerSequence_T>>>::type;
};

export template <integer_sequence_c IntegerSequence_T>
using integer_sequence_sort_t = typename integer_sequence_sort<IntegerSequence_T>::type;

}   // namespace ddge::util::meta
