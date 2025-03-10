module;

#include <concepts>
#include <type_traits>

export module utility.meta.type_traits.integer_sequence.is_integer_sequence;

namespace util::meta {

export template <typename>
struct is_integer_sequence : std::false_type {};

template <
    std::integral Integer_T,
    template <typename, Integer_T...> typename IntegerSequence_T,
    Integer_T... integers_T>
struct is_integer_sequence<IntegerSequence_T<Integer_T, integers_T...>> : std::true_type {
};

export template <typename T>
constexpr bool is_integer_sequence_v = is_integer_sequence<T>::value;

}   // namespace util::meta

#ifdef ENGINE_ENABLE_STATIC_TESTS

template <typename, int...>
struct IntegerSequence {};

static_assert(util::meta::is_integer_sequence_v<IntegerSequence<int>>);
static_assert(util::meta::is_integer_sequence_v<IntegerSequence<int, 1, 0>>);

#endif
