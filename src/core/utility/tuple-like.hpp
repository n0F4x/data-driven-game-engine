#pragma once

#include <tuple>

namespace core {

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

template <class T>
concept tuple_like = is_tuple_like_v<std::remove_cvref_t<T>>;

template <typename T>
concept pair_like = tuple_like<T> && std::tuple_size_v<std::remove_cvref_t<T>> == 2;

}   // namespace core
