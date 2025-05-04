#include <type_traits>

import utility.meta.type_traits.integer_sequence.integer_sequence_sort;

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
