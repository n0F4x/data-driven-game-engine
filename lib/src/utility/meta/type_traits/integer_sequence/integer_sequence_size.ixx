module;

#include <cstdint>

export module utility.meta.type_traits.integer_sequence.integer_sequence_size;

import utility.meta.concepts.integer_sequence.integer_sequence;

namespace util::meta {

export template <integer_sequence_c IntegerSequence_T>
struct integer_sequence_size;

template <
    template <typename T_, T_...> typename IntegerSequence_T,
    typename Integer_T,
    Integer_T... integers_T>
struct integer_sequence_size<IntegerSequence_T<Integer_T, integers_T...>> {
    constexpr static size_t value = sizeof...(integers_T);
};

export template <integer_sequence_c IntegerSequence_T>
inline constexpr size_t integer_sequence_size_v =
    integer_sequence_size<IntegerSequence_T>::value;

}   // namespace util::meta
