#include <type_traits>

import utility.meta.type_traits.integer_sequence.index_sequence_filter;

namespace {
template <typename Integer, Integer...>
struct IntegerSequence {};
}   // namespace

template <size_t I>
struct Even {
    constexpr static bool value = I % 2 == 0;
};

static_assert(std::is_same_v<
              util::meta::index_sequence_filter_t<IntegerSequence<size_t>, Even>,
              IntegerSequence<size_t>>);
static_assert(std::is_same_v<
              util::meta::
                  index_sequence_filter_t<IntegerSequence<size_t, 0, 1, 2, 3, 4>, Even>,
              IntegerSequence<size_t, 0, 2, 4>>);
