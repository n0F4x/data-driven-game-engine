#include <type_traits>

import utility.meta.type_traits.integer_sequence.integer_sequence_to;

namespace {
template <typename T, T...>
struct Sequence0 {};

template <typename T, T...>
struct Sequence1 {};
}   // namespace

static_assert(std::is_same_v<
              util::meta::integer_sequence_to_t<Sequence0<int, 0, 1>, Sequence1>,
              Sequence1<int, 0, 1>>);
