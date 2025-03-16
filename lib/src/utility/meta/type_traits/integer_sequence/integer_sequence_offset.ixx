module;

#include <cstdint>
#include <tuple>

export module utility.meta.type_traits.integer_sequence.integer_sequence_offset;

import utility.meta.concepts.integer_sequence.integer_sequence;

namespace util::meta {

export template <integer_sequence_c IntegerSequence_T, intmax_t offset_T>
struct integer_sequence_offset;

template <
    template <typename T, T...> typename IntegerSequence_T,
    typename Integer_T,
    Integer_T... ints_T,
    intmax_t offset_T>
struct integer_sequence_offset<IntegerSequence_T<Integer_T, ints_T...>, offset_T> {
    using type = IntegerSequence_T<Integer_T, ints_T + offset_T...>;
};

export template <integer_sequence_c IntegerSequence_T, intmax_t offset_T>
using offset_integer_sequence_t =
    typename integer_sequence_offset<IntegerSequence_T, offset_T>::type;

}   // namespace util::meta
