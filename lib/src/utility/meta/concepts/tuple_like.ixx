module;

#include <array>
#include <ranges>
#include <tuple>

export module utility.meta.concepts.tuple_like;

namespace util::meta {

template <class T>
constexpr inline bool is_tuple_like_v = false;

template <class... Elems>
constexpr inline bool is_tuple_like_v<std::tuple<Elems...>> = true;

template <class T1, class T2>
constexpr inline bool is_tuple_like_v<std::pair<T1, T2>> = true;

template <class T, size_t N>
constexpr inline bool is_tuple_like_v<std::array<T, N>> = true;

template <class Iterator_T, class Sentinel_T, std::ranges::subrange_kind subrange_kind_T>
constexpr inline bool
    is_tuple_like_v<std::ranges::subrange<Iterator_T, Sentinel_T, subrange_kind_T>> = true;

export template <class T>
concept tuple_like_c = is_tuple_like_v<std::remove_cvref_t<T>>;

export template <typename T>
concept pair_like_c = tuple_like_c<T> && std::tuple_size_v<std::remove_cvref_t<T>> == 2;

}   // namespace util::meta
