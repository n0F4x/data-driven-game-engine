module;

#include <ranges>
#include <tuple>

/////////////////////////////////////////////////////
// Tuple-like concept                              //
// Reference: https://stackoverflow.com/a/78465321 //
/////////////////////////////////////////////////////
export module utility.meta.concepts.tuple_like;

namespace util::meta {
// NOLINTBEGIN(readability-identifier-naming)

template <class T>
constexpr inline bool is_tuple_like_v = false;

template <class... Elems>
constexpr inline bool is_tuple_like_v<std::tuple<Elems...>> = true;

template <class T1, class T2>
constexpr inline bool is_tuple_like_v<std::pair<T1, T2>> = true;

template <class T, size_t N>
constexpr inline bool is_tuple_like_v<std::array<T, N>> = true;

template <class It, class Sent, std::ranges::subrange_kind Kind>
constexpr inline bool is_tuple_like_v<std::ranges::subrange<It, Sent, Kind>> = true;

export template <class T>
concept tuple_like_c = is_tuple_like_v<std::remove_cvref_t<T>>;

export template <typename T>
concept pair_like_c = tuple_like_c<T> && std::tuple_size_v<std::remove_cvref_t<T>> == 2;

// NOLINTEND(readability-identifier-naming)
}   // namespace util::meta
