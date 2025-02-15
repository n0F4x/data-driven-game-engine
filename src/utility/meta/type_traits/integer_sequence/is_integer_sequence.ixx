module;

#include <concepts>
#include <type_traits>

export module utility.meta.type_traits.integer_sequence.is_integer_sequence;

namespace util::meta {

export template <typename>
struct is_integer_sequence : std::false_type {};

template <template <typename, size_t...> typename T, std::integral Integer_T, size_t... Indices_V>
struct is_integer_sequence<T<Integer_T, Indices_V...>> : std::true_type {};

export template <typename T>
constexpr bool is_integer_sequence_v = is_integer_sequence<T>::value;

}   // namespace util::meta
