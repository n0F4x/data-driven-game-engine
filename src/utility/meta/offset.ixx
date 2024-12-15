module;

#include <tuple>

// reference: https://devblogs.microsoft.com/oldnewthing/20200625-00/?p=103903
export module utility.meta.offset;

namespace utils::meta {
// NOLINTBEGIN(readability-identifier-naming)

template <size_t N, typename Seq>
struct offset;

template <size_t N, std::size_t... Ints>
struct offset<N, std::index_sequence<Ints...>> {
    using type = std::index_sequence<Ints + N...>;
};

export template <size_t N, typename Seq>
using offset_t = typename offset<N, Seq>::type;

// NOLINTEND(readability-identifier-naming)
}   // namespace utils::meta
