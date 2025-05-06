module;

#include <cstdint>

export module utility.meta.type_traits.integer_sequence.integer_sequence_at;

import utility.meta.concepts.integer_sequence.integer_sequence;

namespace util::meta {

export template <integer_sequence_c IntegerSequence, size_t index_T>
struct integer_sequence_at;

template <
    template <typename T_, T_...> typename IntegerSequence,
    typename Integer_T,
    Integer_T... integers_T,
    size_t index_T>
struct integer_sequence_at<IntegerSequence<Integer_T, integers_T...>, index_T> {
    constexpr static Integer_T value = integers_T...[index_T];
};

export template <integer_sequence_c IntegerSequence, size_t index_T>
constexpr inline auto integer_sequence_at_v =
    integer_sequence_at<IntegerSequence, index_T>::value;

}   // namespace util::meta
