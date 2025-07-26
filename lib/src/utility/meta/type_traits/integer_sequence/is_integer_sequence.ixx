module;

#include <concepts>
#include <type_traits>

export module utility.meta.type_traits.integer_sequence.is_integer_sequence;

namespace util::meta {

export template <typename>
struct is_integer_sequence : std::false_type {};

template <
    template <typename T_, T_...> typename IntegerSequence_T,
    std::integral Integer_T,
    Integer_T... integers_T>
struct is_integer_sequence<IntegerSequence_T<Integer_T, integers_T...>> : std::true_type {
};

export template <typename T>
inline constexpr bool is_integer_sequence_v = is_integer_sequence<T>::value;

}   // namespace util::meta
