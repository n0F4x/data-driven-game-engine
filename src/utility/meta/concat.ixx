module;

#include <tuple>

// reference: https://devblogs.microsoft.com/oldnewthing/20200625-00/?p=103903
export module utility.meta.concat;

namespace utils::meta {
// NOLINTBEGIN(readability-identifier-naming)

template <typename Seq1, typename Seq>
struct concat;

template <size_t... Ints1, size_t... Ints2>
struct concat<std::index_sequence<Ints1...>, std::index_sequence<Ints2...>> {
    using type = std::index_sequence<Ints1..., Ints2...>;
};

export template <typename Seq1, typename Seq2>
using concat_t = typename concat<Seq1, Seq2>::type;

// NOLINTEND(readability-identifier-naming)
}   // namespace utils::meta
