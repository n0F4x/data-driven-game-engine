module;

#include <cstdint>
#include <tuple>

// reference: https://devblogs.microsoft.com/oldnewthing/20200625-00/?p=103903
export module utility.type_traits.integer_sequence.offset_integer_sequence;

namespace util::meta {
// NOLINTBEGIN(readability-identifier-naming)

template <typename IntegerSequence_T, intmax_t Offset_V>
struct offset_integer_sequence;

template <typename Integer_T, intmax_t Offset_V, Integer_T... Ints_V>
struct offset_integer_sequence<std::integer_sequence<Integer_T, Ints_V...>, Offset_V> {
    using type = std::integer_sequence<Integer_T, Ints_V + Offset_V...>;
};

export template <typename IntegerSequence_T, intmax_t Offset_V>
using offset_integer_sequence_t =
    typename offset_integer_sequence<IntegerSequence_T, Offset_V>::type;

// NOLINTEND(readability-identifier-naming)
}   // namespace util::meta
