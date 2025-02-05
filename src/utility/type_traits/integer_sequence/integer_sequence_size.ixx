module;

#include <concepts>

export module utility.type_traits.integer_sequence.integer_sequence_size;

namespace util::meta {

export template <typename IntegerSequence_T>
struct integer_sequence_size;

template <
    template <typename, size_t...> typename IntegerSequence_T,
    std::integral Integer_T,
    size_t... Integers_V>
struct integer_sequence_size<IntegerSequence_T<Integer_T, Integers_V...>> {
    constexpr static size_t value = sizeof...(Integers_V);
};

export template <typename IntegerSequence_T>
constexpr size_t integer_sequence_size_v = integer_sequence_size<IntegerSequence_T>::value;

}   // namespace util::meta
