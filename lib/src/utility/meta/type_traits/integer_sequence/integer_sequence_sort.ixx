module;

#include <algorithm>
#include <array>

export module utility.meta.type_traits.integer_sequence.integer_sequence_sort;

import utility.meta.concepts.integer_sequence.integer_sequence;
import utility.meta.type_traits.integer_sequence.integer_sequence_size;

template <typename IntegerSequence_T, typename IndexSequence_T>
struct integer_sequence_sort_impl;

template <
    template <typename T_, T_...> typename IntegerSequence_T,
    typename Integer_T,
    Integer_T... integers_T,
    size_t... indices_T>
struct integer_sequence_sort_impl<
    IntegerSequence_T<Integer_T, integers_T...>,
    std::index_sequence<indices_T...>> {
    constexpr static std::array sorted_array = [] {
        std::array<Integer_T, sizeof...(integers_T)> result{ integers_T... };
        std::ranges::sort(result);
        return result;
    }();

    using type = IntegerSequence_T<Integer_T, sorted_array[indices_T]...>;
};

namespace util::meta {

export template <integer_sequence_c IntegerSequence_T>
struct integer_sequence_sort {
    using type = typename integer_sequence_sort_impl<
        IntegerSequence_T,
        std::make_index_sequence<integer_sequence_size_v<IntegerSequence_T>>>::type;
};

export template <integer_sequence_c IntegerSequence_T>
using integer_sequence_sort_t = typename integer_sequence_sort<IntegerSequence_T>::type;

}   // namespace util::meta

module :private;

#ifdef ENGINE_ENABLE_STATIC_TESTS

// TODO: remove unnamed namespace when Clang allows it
namespace {
template <typename, int...>
struct IntegerSequence {};
}   // namespace

static_assert(std::is_same_v<
              util::meta::integer_sequence_sort_t<IntegerSequence<int>>,
              IntegerSequence<int>>);
static_assert(std::is_same_v<
              util::meta::integer_sequence_sort_t<IntegerSequence<int, 1, 2, 0>>,
              IntegerSequence<int, 0, 1, 2>>);

#endif
