module;

#include <type_traits>

export module utility.meta.type_traits.integer_sequence.is_index_sequence;

namespace util::meta {

export template <typename>
struct is_index_sequence : std::false_type {};

template <template <size_t...> typename T, size_t... Indices_V>
struct is_index_sequence<T<Indices_V...>> : std::true_type {};

export template <typename T>
constexpr bool is_index_sequence_v = is_index_sequence<T>::value;

}   // namespace util::meta
