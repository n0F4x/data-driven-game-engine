module;

#include <tuple>

// reference: https://devblogs.microsoft.com/oldnewthing/20200625-00/?p=103903
export module utility.meta.offset;

namespace utils::meta {
// NOLINTBEGIN(readability-identifier-naming)

template <typename Sequence_T, size_t Offset_V>
struct offset;

template <size_t Offset_V, size_t... Ints_V>
struct offset<std::index_sequence<Ints_V...>, Offset_V> {
    using type = std::index_sequence<Ints_V + Offset_V...>;
};

export template <typename Sequence_T, size_t Offset_V>
using offset_t = typename offset<Sequence_T, Offset_V>::type;

// NOLINTEND(readability-identifier-naming)
}   // namespace utils::meta
