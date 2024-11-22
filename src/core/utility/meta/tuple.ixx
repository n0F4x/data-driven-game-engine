module;

#include <tuple>

// reference: https://devblogs.microsoft.com/oldnewthing/20200625-00/?p=103903
export module core.utility.meta.tuple;

namespace core::meta {
// NOLINTBEGIN(readability-identifier-naming)

template <size_t N, typename Seq>
struct offset_sequence;

template <size_t N, std::size_t... Ints>
struct offset_sequence<N, std::index_sequence<Ints...>> {
    using type = std::index_sequence<Ints + N...>;
};

export template <size_t N, typename Seq>
using offset_sequence_t = typename offset_sequence<N, Seq>::type;

template <typename Seq1, typename Seq>
struct cat_sequence;

template <size_t... Ints1, size_t... Ints2>
struct cat_sequence<std::index_sequence<Ints1...>, std::index_sequence<Ints2...>> {
    using type = std::index_sequence<Ints1..., Ints2...>;
};

export template <typename Seq1, typename Seq2>
using cat_sequence_t = typename cat_sequence<Seq1, Seq2>::type;

// NOLINTEND(readability-identifier-naming)
}   // namespace core::meta
