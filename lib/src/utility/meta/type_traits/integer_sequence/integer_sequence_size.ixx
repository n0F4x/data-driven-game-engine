module;

#include <cstdint>

export module utility.meta.type_traits.integer_sequence.integer_sequence_size;

import utility.meta.concepts.integer_sequence.integer_sequence;

namespace util::meta {

export template <integer_sequence_c IntegerSequence_T>
struct integer_sequence_size;

template <
    typename Integer_T,
    template <typename, Integer_T...> typename IntegerSequence_T,
    Integer_T... integers_T>
struct integer_sequence_size<IntegerSequence_T<Integer_T, integers_T...>> {
    constexpr static size_t value = sizeof...(integers_T);
};

export template <integer_sequence_c IntegerSequence_T>
constexpr size_t integer_sequence_size_v = integer_sequence_size<IntegerSequence_T>::value;

}   // namespace util::meta
