export module utility.meta.type_traits.integer_sequence.integer_sequence_concat;

import utility.meta.concepts.integer_sequence.integer_sequence;

namespace util::meta {

template <
    integer_sequence_c First_T,
    integer_sequence_c Second_T,
    integer_sequence_c... IntegerSequencess_T>
struct integer_sequence_concat;

template <
    template <typename T, T...> typename IntegerSequence_T,
    typename Integer_T,
    Integer_T... left_ints_T,
    Integer_T... right_ints_T>
struct integer_sequence_concat<
    IntegerSequence_T<Integer_T, left_ints_T...>,
    IntegerSequence_T<Integer_T, right_ints_T...>> {
    using type = IntegerSequence_T<Integer_T, left_ints_T..., right_ints_T...>;
};

template <integer_sequence_c First_T, integer_sequence_c Second_T, integer_sequence_c... Rest_T>
    requires(sizeof...(Rest_T) != 0)
struct integer_sequence_concat<First_T, Second_T, Rest_T...>
    : integer_sequence_concat<
          typename integer_sequence_concat<First_T, Second_T>::type,
          Rest_T...> {};

export template <integer_sequence_c... IntegerSequencess_T>
using integer_sequence_concat_t =
    typename integer_sequence_concat<IntegerSequencess_T...>::type;

}   // namespace util::meta
