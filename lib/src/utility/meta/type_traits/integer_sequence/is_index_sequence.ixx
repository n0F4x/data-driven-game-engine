module;

#include <type_traits>

export module ddge.utility.meta.type_traits.integer_sequence.is_index_sequence;

namespace ddge::util::meta {

export template <typename>
struct is_index_sequence : std::false_type {};

template <template <typename T_, T_...> typename IntegerSequence_T, std::size_t... indices_T>
struct is_index_sequence<IntegerSequence_T<std::size_t, indices_T...>> : std::true_type {
};

export template <typename T>
inline constexpr bool is_index_sequence_v = is_index_sequence<T>::value;

}   // namespace ddge::util::meta
