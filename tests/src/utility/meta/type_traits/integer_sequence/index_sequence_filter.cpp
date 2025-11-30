#include <type_traits>

import ddge.utility.meta.type_traits.integer_sequence.index_sequence_filter;

namespace {

template <typename Integer, Integer...>
struct IntegerSequence {};

template <std::size_t I>
struct IsEven {
    constexpr static bool value = I % 2 == 0;
};

}   // namespace

static_assert(
    std::is_same_v<
        ddge::util::meta::index_sequence_filter_t<IntegerSequence<std::size_t>, IsEven>,
        IntegerSequence<std::size_t>>
);
static_assert(
    std::is_same_v<
        ddge::util::meta::
            index_sequence_filter_t<IntegerSequence<std::size_t, 0, 1, 2, 3, 4>, IsEven>,
        IntegerSequence<std::size_t, 0, 2, 4>>
);
