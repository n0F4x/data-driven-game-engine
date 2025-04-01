export module utility.meta.type_traits.integer_sequence.integer_sequence_to;

import utility.meta.concepts.integer_sequence.integer_sequence;

namespace util::meta {

export template <integer_sequence_c From_T, template <typename T, T...> typename To_T>
struct integer_sequence_to;

template <
    template <typename T, T...> typename From_T,
    typename Integer_T,
    Integer_T... integers_T,
    template <typename T, T...> typename To_T>
struct integer_sequence_to<From_T<Integer_T, integers_T...>, To_T> {
    using type = To_T<Integer_T, integers_T...>;
};

export template <integer_sequence_c From_T, template <typename T, T...> typename To_T>
using integer_sequence_to_t = typename integer_sequence_to<From_T, To_T>::type;

}   // namespace util::meta
